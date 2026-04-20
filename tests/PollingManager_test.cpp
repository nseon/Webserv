#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "PollingManager.hpp"
#include "ASocket.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

// Mock socket class for testing - uses one end of a socketpair
class MockSocket : public ASocket
{
public:
	int _peerFd; // The other end of the socketpair (to write to)

	MockSocket(int fd, int peerFd) : ASocket(fd), _peerFd(peerFd) {}

	~MockSocket()
	{
		if (_peerFd >= 0)
			close(_peerFd);
	}

	virtual int socketBehavior(void*) override
	{
		char buf[256];
		ssize_t n = recv(_socketFd, buf, sizeof(buf), 0);
		(void)n;
		return 0;
	}

	// Simulate incoming data by writing to the peer end
	void simulateIncomingData(const char* data)
	{
		send(_peerFd, data, strlen(data), 0);
	}
};

// Helper to create a connected socket pair and return a MockSocket
MockSocket* createMockSocket()
{
	int fds[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1)
		return nullptr;
	return new MockSocket(fds[0], fds[1]);
}

TEST_CASE("PollingManager construction")
{
	CHECK_NOTHROW(PollingManager());
}

TEST_CASE("PollingManager add and remove client socket")
{
	PollingManager pm;
	int fds[2];
	REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);

	CHECK_NOTHROW(pm.addClientSocket(fds[0]));
	CHECK_NOTHROW(pm.removeSocket(fds[0]));

	close(fds[1]); // Clean up peer fd
}

TEST_CASE("PollingManager poll returns ready sockets")
{
	PollingManager pm;
	int fds1[2], fds2[2];

	REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, fds1) == 0);
	REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, fds2) == 0);

	pm.addClientSocket(fds1[0]);
	pm.addClientSocket(fds2[0]);

	// Simulate data on socket 1 only
	const char* testData = "test";
	send(fds1[1], testData, strlen(testData), 0);

	std::vector<ASocket*> ready = pm.poll();

	// At least one socket should be ready
	CHECK(ready.size() >= 1);

	// Check that the ready socket is the one we wrote to
	bool found = false;
	for (size_t i = 0; i < ready.size(); i++)
	{
		if (ready[i] && ready[i]->getFd() == fds1[0])
			found = true;
	}
	CHECK(found);

	// Cleanup
	pm.removeSocket(fds1[0]);
	pm.removeSocket(fds2[0]);
	close(fds1[1]);
	close(fds2[1]);
}

TEST_CASE("PollingManager poll with multiple ready sockets")
{
	PollingManager pm;
	int fds1[2], fds2[2];

	REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, fds1) == 0);
	REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, fds2) == 0);

	pm.addClientSocket(fds1[0]);
	pm.addClientSocket(fds2[0]);

	// Simulate data on both sockets
	const char* testData = "hello";
	send(fds1[1], testData, strlen(testData), 0);
	send(fds2[1], testData, strlen(testData), 0);

	std::vector<ASocket*> ready = pm.poll();

	// Both sockets should be ready
	CHECK(ready.size() >= 2);

	// Cleanup
	pm.removeSocket(fds1[0]);
	pm.removeSocket(fds2[0]);
	close(fds1[1]);
	close(fds2[1]);
}

TEST_CASE("PollingManager handles socket disconnection")
{
	PollingManager pm;
	int fds[2];

	REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);

	pm.addClientSocket(fds[0]);

	// Close the peer end to simulate disconnection
	close(fds[1]);

	// poll should return the socket as ready (with EPOLLHUP or similar)
	std::vector<ASocket*> ready = pm.poll();
	CHECK(ready.size() >= 1);

	pm.removeSocket(fds[0]);
}
