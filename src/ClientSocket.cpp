#include "ClientSocket.hpp"
#include "PollingManager.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

#include <iostream> //tmp

#ifndef RECV_SIZE
# define RECV_SIZE 1024
#endif

ClientSocket::ClientSocket(void) {}

ClientSocket::ClientSocket(int fd):
ASocket (fd) {}

ClientSocket::~ClientSocket(void) {}

ClientSocket*	ClientSocket::clone(void) const
{
	return (new ClientSocket(*this));
}

int	ClientSocket::socketBehavior(void *pm)
{
	int		socketFd = this->socketFd_;
	char	msg[RECV_SIZE];
	ssize_t	msg_length;

	msg_length = recv(this->socketFd_, msg, RECV_SIZE, 0);
	if (msg_length <= 0)
	{
		reinterpret_cast<PollingManager*>(pm)->removeSocket(this->socketFd_);
		std::strcpy(msg, "disconected.\n");
	}
	else
	{
		msg[msg_length] = 0;
	}
	std::cout << socketFd << ' ' << msg << std::flush;
	return (0);
}
