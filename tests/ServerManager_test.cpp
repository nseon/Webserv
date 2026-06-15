// =============================================================================
// test_ServerManager.cpp
// Unit tests for ServerManager using doctest
//
// Compile example (adapt paths to your project):
//   g++ -std=c++98 -Wall -Wextra -pthread \
//       test_ServerManager.cpp \
//       ServerManager.cpp ClientSocket.cpp ASocket.cpp \
//       PollingManager.cpp Server.cpp ListenerSocket.cpp \
//       Ablock.cpp Location.cpp Logger.cpp \
//       -I. -o test_server_manager && ./test_server_manager
//
// Note: doctest.h must be in your include path.
// =============================================================================

#include "doctest.h"

#include "manager/ServerManager.hpp"
#include "config/Server.hpp"

#include <sys/socket.h>   // socketpair()
#include <netinet/in.h>   // sockaddr_in
#include <unistd.h>       // close()
#include <cstring>        // memset()
#include <stdexcept>
#include <vector>

// =============================================================================
// Helpers
// =============================================================================

// A zeroed client address — addClientSocket now takes (fd, addr, server).
static struct sockaddr_in zero_addr()
{
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    return addr;
}

// Returns a ServerManager with NO servers.
// No bind(), no listen(), no real network — safe for unit tests.
static ServerManager make_empty_manager()
{
    return ServerManager(std::vector<Server>());
}

struct SockPair
{
    int client; // passed to addClientSocket — owned by ClientSocket after that
    int peer;   // kept by the test — must be closed manually
};

// Creates a Unix socketpair.
// sp.client is given to ServerManager::addClientSocket().
// sp.peer must be close()d by the test after the manager is destroyed.
static SockPair make_sock_pair()
{
    SockPair sp;
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1)
        throw std::runtime_error("socketpair() failed");
    sp.client = fds[0];
    sp.peer   = fds[1];
    return sp;
}

// =============================================================================
// TEST SUITE — Construction
// =============================================================================

TEST_SUITE("ServerManager — Construction")
{
    TEST_CASE("Construction with an empty server list does not throw")
    {
        CHECK_NOTHROW(make_empty_manager());
    }
}

// =============================================================================
// TEST SUITE — addClientSocket
// =============================================================================

TEST_SUITE("ServerManager — addClientSocket")
{
    TEST_CASE("Adding a valid fd does not throw")
    {
        ServerManager sm = make_empty_manager();
        SockPair sp = make_sock_pair();

        CHECK_NOTHROW(sm.addClientSocket(sp.client, zero_addr(), NULL));
        // sp.client is now owned by the ClientSocket inside sm

        close(sp.peer);
        // sm destructor deletes the ClientSocket and closes sp.client
    }

    TEST_CASE("Adding multiple distinct fds does not throw")
    {
        ServerManager sm = make_empty_manager();
        SockPair sp1 = make_sock_pair();
        SockPair sp2 = make_sock_pair();
        SockPair sp3 = make_sock_pair();

        CHECK_NOTHROW(sm.addClientSocket(sp1.client, zero_addr(), NULL));
        CHECK_NOTHROW(sm.addClientSocket(sp2.client, zero_addr(), NULL));
        CHECK_NOTHROW(sm.addClientSocket(sp3.client, zero_addr(), NULL));

        close(sp1.peer);
        close(sp2.peer);
        close(sp3.peer);
    }
}

// =============================================================================
// TEST SUITE — removeClientSocket
// =============================================================================

TEST_SUITE("ServerManager — removeClientSocket")
{
    TEST_CASE("Removing a previously added client does not throw")
    {
        ServerManager sm = make_empty_manager();
        SockPair sp = make_sock_pair();

        sm.addClientSocket(sp.client, zero_addr(), NULL);
        CHECK_NOTHROW(sm.removeClientSocket(sp.client));
        // ClientSocket dtor closed sp.client

        close(sp.peer);
    }

    TEST_CASE("Removing an unknown fd is a no-op and does not throw")
    {
        // removeClientSocket() guards with findClient() != end() before acting
        ServerManager sm = make_empty_manager();
        CHECK_NOTHROW(sm.removeClientSocket(9999));
    }

    TEST_CASE("Removing the same fd twice does not throw")
    {
        ServerManager sm = make_empty_manager();
        SockPair sp = make_sock_pair();

        sm.addClientSocket(sp.client, zero_addr(), NULL);
        sm.removeClientSocket(sp.client); // first: client deleted, fd closed
        CHECK_NOTHROW(sm.removeClientSocket(sp.client)); // second: no-op

        close(sp.peer);
    }

    TEST_CASE("Removing one client does not affect others")
    {
        ServerManager sm = make_empty_manager();
        SockPair sp1 = make_sock_pair();
        SockPair sp2 = make_sock_pair();

        sm.addClientSocket(sp1.client, zero_addr(), NULL);
        sm.addClientSocket(sp2.client, zero_addr(), NULL);

        sm.removeClientSocket(sp1.client);

        // sp2 is still tracked; a second remove must succeed cleanly
        CHECK_NOTHROW(sm.removeClientSocket(sp2.client));

        close(sp1.peer);
        close(sp2.peer);
    }

    TEST_CASE("All clients can be removed one by one without throw")
    {
        const int N = 10;
        ServerManager sm = make_empty_manager();
        SockPair pairs[N];

        for (int i = 0; i < N; ++i)
        {
            pairs[i] = make_sock_pair();
            sm.addClientSocket(pairs[i].client, zero_addr(), NULL);
        }

        for (int i = 0; i < N; ++i)
        {
            CHECK_NOTHROW(sm.removeClientSocket(pairs[i].client));
            close(pairs[i].peer);
        }
    }
}

// =============================================================================
// TEST SUITE — add / remove round-trips
// =============================================================================

TEST_SUITE("ServerManager — add/remove round-trips")
{
    TEST_CASE("Add, remove, then add a new fd does not throw")
    {
        ServerManager sm = make_empty_manager();
        SockPair sp1 = make_sock_pair();
        SockPair sp2 = make_sock_pair();

        sm.addClientSocket(sp1.client, zero_addr(), NULL);
        sm.removeClientSocket(sp1.client); // sp1.client closed internally

        CHECK_NOTHROW(sm.addClientSocket(sp2.client, zero_addr(), NULL));

        close(sp1.peer);
        close(sp2.peer);
    }

    TEST_CASE("Interleaved add and remove on many clients does not throw")
    {
        const int N = 8;
        ServerManager sm = make_empty_manager();
        SockPair pairs[N];

        // Add all
        for (int i = 0; i < N; ++i)
        {
            pairs[i] = make_sock_pair();
            sm.addClientSocket(pairs[i].client, zero_addr(), NULL);
        }

        // Remove even-indexed clients
        for (int i = 0; i < N; i += 2)
        {
            CHECK_NOTHROW(sm.removeClientSocket(pairs[i].client));
            close(pairs[i].peer);
        }

        // Remove odd-indexed clients
        for (int i = 1; i < N; i += 2)
        {
            CHECK_NOTHROW(sm.removeClientSocket(pairs[i].client));
            close(pairs[i].peer);
        }
    }
}
