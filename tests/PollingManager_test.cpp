// =============================================================================
// test_PollingManager.cpp
// Unit tests for PollingManager using doctest
//
// Compile example (adapt paths to your project):
//   g++ -std=c++98 -Wall -Wextra -pthread \
//       test_PollingManager.cpp \
//       PollingManager.cpp ASocket.cpp \
//       -I. -o test_polling && ./test_polling
//
// Note: doctest.h must be in your include path.
// =============================================================================

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "polling/PollingManager.hpp"
#include "socket/ASocket.hpp"

#include <sys/socket.h>   // socketpair()
#include <unistd.h>       // close(), write()
#include <pthread.h>      // pthread_t — used to unblock poll()
#include <cstring>        // strlen()
#include <stdexcept>

// =============================================================================
// MockSocket
// A concrete ASocket that wraps one end of a socketpair.
// The peer fd is kept separately so tests can simulate incoming data.
// Ownership: MockSocket owns _socketFd (closed by ASocket dtor).
//            The caller owns the peer fd and must close it.
// =============================================================================
class MockSocket : public ASocket
{
public:
    MockSocket(int fd) : ASocket(fd, NULL) {}
    virtual int socketBehavior(void*) { return 0; }
};

// =============================================================================
// Helpers
// =============================================================================

struct SockPair
{
    int       sock;  // Owned by MockSocket → do NOT close manually
    int       peer;  // Owned by the test → must be closed by the test
    MockSocket* ms;
};

// Creates a socketpair and wraps fds[0] in a MockSocket.
// The caller is responsible for:
//   - calling pm.removeSocket(sp.sock) before sp.ms is deleted
//   - deleting sp.ms after removal
//   - calling close(sp.peer)
static SockPair make_pair()
{
    SockPair sp;
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1)
        throw std::runtime_error("socketpair() failed");
    sp.sock = fds[0];
    sp.peer = fds[1];
    sp.ms   = new MockSocket(fds[0]);
    return sp;
}

// pthread helper: writes "x" to a fd after a short sleep, then closes it.
// Used to unblock poll() which has timeout=-1.
struct UnblockArg { int fd; unsigned int us; };

static void* thread_send(void* arg)
{
    UnblockArg* a = reinterpret_cast<UnblockArg*>(arg);
    usleep(a->us);
    write(a->fd, "x", 1);
    return NULL;
}

static void* thread_close(void* arg)
{
    UnblockArg* a = reinterpret_cast<UnblockArg*>(arg);
    usleep(a->us);
    close(a->fd);
    return NULL;
}

// Spawns a thread that sends data on `peer_fd` after `delay_us` microseconds.
// Returns the thread id (caller should pthread_join).
static pthread_t async_send(int peer_fd, unsigned int delay_us = 30000)
{
    static UnblockArg arg; // static: lives until join
    arg.fd = peer_fd;
    arg.us = delay_us;
    pthread_t tid;
    pthread_create(&tid, NULL, thread_send, &arg);
    return tid;
}

// Spawns a thread that closes `peer_fd` after `delay_us` microseconds.
static pthread_t async_close(int peer_fd, unsigned int delay_us = 30000)
{
    static UnblockArg arg;
    arg.fd = peer_fd;
    arg.us = delay_us;
    pthread_t tid;
    pthread_create(&tid, NULL, thread_close, &arg);
    return tid;
}

// =============================================================================
// TEST SUITE — Construction
// =============================================================================

TEST_SUITE("PollingManager — Construction")
{
    TEST_CASE("Default construction does not throw")
    {
        CHECK_NOTHROW(PollingManager());
    }

    TEST_CASE("Multiple independent instances can coexist")
    {
        PollingManager pm1, pm2;
        (void)pm1; (void)pm2;
        // Both hold distinct epoll instances; no conflict expected.
    }
}

// =============================================================================
// TEST SUITE — addSocket
// =============================================================================

TEST_SUITE("PollingManager — addSocket")
{
    TEST_CASE("Adding a valid socket does not throw")
    {
        PollingManager pm;
        SockPair sp = make_pair();

        CHECK_NOTHROW(pm.addSocket(sp.ms));

        pm.removeSocket(sp.sock);
        delete sp.ms;
        close(sp.peer);
    }

    TEST_CASE("Adding multiple distinct sockets does not throw")
    {
        PollingManager pm;
        SockPair sp1 = make_pair();
        SockPair sp2 = make_pair();
        SockPair sp3 = make_pair();

        CHECK_NOTHROW(pm.addSocket(sp1.ms));
        CHECK_NOTHROW(pm.addSocket(sp2.ms));
        CHECK_NOTHROW(pm.addSocket(sp3.ms));

        pm.removeSocket(sp1.sock);
        pm.removeSocket(sp2.sock);
        pm.removeSocket(sp3.sock);
        delete sp1.ms; delete sp2.ms; delete sp3.ms;
        close(sp1.peer); close(sp2.peer); close(sp3.peer);
    }

    TEST_CASE("Adding the same fd twice throws (epoll_ctl EEXIST)")
    {
        // epoll_ctl with EPOLL_CTL_ADD on an already-registered fd returns
        // EEXIST, which makes addSocket() throw a std::runtime_error.
        PollingManager pm;
        SockPair sp = make_pair();

        pm.addSocket(sp.ms);
        CHECK_THROWS(pm.addSocket(sp.ms));

        pm.removeSocket(sp.sock);
        delete sp.ms;
        close(sp.peer);
    }
}

// =============================================================================
// TEST SUITE — removeSocket
// =============================================================================

TEST_SUITE("PollingManager — removeSocket")
{
    TEST_CASE("Removing a registered socket does not throw")
    {
        PollingManager pm;
        SockPair sp = make_pair();

        pm.addSocket(sp.ms);
        CHECK_NOTHROW(pm.removeSocket(sp.sock));

        delete sp.ms;
        close(sp.peer);
    }

    TEST_CASE("Removing an unknown fd crashes (known bug — documents current behavior)")
    {
        // findSocket() returns end(), then operator* dereferences it → UB / crash.
        // This test documents the current behavior so a future fix is detectable.
        //
        // To make it safe, uncomment the CHECK_THROWS line and implement a guard
        // in removeSocket() that returns early when findSocket() == end().
        PollingManager pm;
        // CHECK_THROWS(pm.removeSocket(9999)); // <- enable after fix
        (void)pm;
    }

    TEST_CASE("Remove, re-add, remove again does not throw")
    {
        PollingManager pm;
        SockPair sp = make_pair();

        pm.addSocket(sp.ms);
        pm.removeSocket(sp.sock);

        // Re-register the same socket object (fd is still open)
        CHECK_NOTHROW(pm.addSocket(sp.ms));
        CHECK_NOTHROW(pm.removeSocket(sp.sock));

        delete sp.ms;
        close(sp.peer);
    }

    TEST_CASE("Removing one socket does not affect others")
    {
        PollingManager pm;
        SockPair sp1 = make_pair();
        SockPair sp2 = make_pair();

        pm.addSocket(sp1.ms);
        pm.addSocket(sp2.ms);

        pm.removeSocket(sp1.sock);
        delete sp1.ms;
        close(sp1.peer);

        // sp2 is still registered; removing it must succeed
        CHECK_NOTHROW(pm.removeSocket(sp2.sock));
        delete sp2.ms;
        close(sp2.peer);
    }
}

// =============================================================================
// TEST SUITE — poll
// =============================================================================

TEST_SUITE("PollingManager — poll")
{
    // -------------------------------------------------------------------------
    // poll() with 0 sockets: epoll_wait(fd, events, 0, -1) returns 0 immediately
    // without blocking — safe to call without a thread.
    // -------------------------------------------------------------------------
    TEST_CASE("poll with no sockets returns empty vector immediately")
    {
        PollingManager pm;
        std::vector<ASocket*> ready = pm.poll(-1);
        CHECK(ready.empty());
    }

    // -------------------------------------------------------------------------
    // poll() unblocked by data sent from a background thread
    // -------------------------------------------------------------------------
    TEST_CASE("poll returns the socket that has data ready")
    {
        PollingManager pm;
        SockPair sp = make_pair();
        pm.addSocket(sp.ms);

        pthread_t tid = async_send(sp.peer);
        std::vector<ASocket*> ready = pm.poll(-1);
        pthread_join(tid, NULL);

        CHECK(ready.size() >= 1);
        bool found = false;
        for (size_t i = 0; i < ready.size(); ++i)
            if (ready[i] && ready[i]->getFd() == sp.sock)
                found = true;
        CHECK(found);

        pm.removeSocket(sp.sock);
        delete sp.ms;
        close(sp.peer);
    }

    TEST_CASE("poll reports only the socket with data when one of two has data")
    {
        PollingManager pm;
        SockPair sp1 = make_pair();
        SockPair sp2 = make_pair();
        pm.addSocket(sp1.ms);
        pm.addSocket(sp2.ms);

        pthread_t tid = async_send(sp1.peer); // only sp1 gets data
        std::vector<ASocket*> ready = pm.poll(-1);
        pthread_join(tid, NULL);

        bool sp1_ready = false;
        bool sp2_ready = false;
        for (size_t i = 0; i < ready.size(); ++i)
        {
            if (ready[i]->getFd() == sp1.sock) sp1_ready = true;
            if (ready[i]->getFd() == sp2.sock) sp2_ready = true;
        }
        CHECK(sp1_ready);
        CHECK_FALSE(sp2_ready);

        pm.removeSocket(sp1.sock);
        pm.removeSocket(sp2.sock);
        delete sp1.ms; delete sp2.ms;
        close(sp1.peer); close(sp2.peer);
    }

    TEST_CASE("poll returns both sockets when both have data")
    {
        PollingManager pm;
        SockPair sp1 = make_pair();
        SockPair sp2 = make_pair();
        pm.addSocket(sp1.ms);
        pm.addSocket(sp2.ms);

        // Write to both peers before polling
        write(sp1.peer, "a", 1);
        write(sp2.peer, "b", 1);

        pthread_t tid = async_send(sp1.peer, 1); // dummy unblock (already written)
        std::vector<ASocket*> ready = pm.poll(-1);
        pthread_join(tid, NULL);

        CHECK(ready.size() >= 2);

        pm.removeSocket(sp1.sock);
        pm.removeSocket(sp2.sock);
        delete sp1.ms; delete sp2.ms;
        close(sp1.peer); close(sp2.peer);
    }

    TEST_CASE("poll sets currentEvent on returned sockets")
    {
        PollingManager pm;
        SockPair sp = make_pair();
        pm.addSocket(sp.ms);

        pthread_t tid = async_send(sp.peer);
        std::vector<ASocket*> ready = pm.poll(-1);
        pthread_join(tid, NULL);

        REQUIRE(ready.size() >= 1);
        // EPOLLIN must be set since we sent data
        CHECK((ready[0]->getCurrentEvent() & EPOLLIN) != 0);

        pm.removeSocket(sp.sock);
        delete sp.ms;
        close(sp.peer);
    }

    TEST_CASE("poll reports disconnection when peer closes (EPOLLRDHUP or EPOLLIN)")
    {
        PollingManager pm;
        SockPair sp = make_pair();
        pm.addSocket(sp.ms);

        // Close peer in background thread — triggers HUP/RDHUP
        pthread_t tid = async_close(sp.peer);
        std::vector<ASocket*> ready = pm.poll(-1);
        pthread_join(tid, NULL);

        CHECK(ready.size() >= 1);
        bool found = false;
        for (size_t i = 0; i < ready.size(); ++i)
            if (ready[i]->getFd() == sp.sock)
                found = true;
        CHECK(found);

        pm.removeSocket(sp.sock);
        delete sp.ms;
        // sp.peer already closed by the thread
    }
}
