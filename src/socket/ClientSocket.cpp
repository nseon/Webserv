#include "ClientSocket.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"
#include <sys/epoll.h>
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

int	ClientSocket::socketBehavior(void *pm)
{
	int		socketFd = this->_socketFd;
	char	msg[RECV_SIZE];
	ssize_t	msg_length;

	if (this->_currentEvent & EPOLLRDHUP)
	{
		reinterpret_cast<ServerManager*>(pm)->removeClientSocket(this->_socketFd);
		std::strcpy(msg, "disconected.\n");
	}
	else
	{
		Logger::info() << "Client " << this->_socketFd << " sended a message : " << std::endl;
		msg_length = recv(this->_socketFd, msg, RECV_SIZE, 0);
		msg[msg_length] = 0;
	}
	std::cout << socketFd << ' ' << msg << std::flush;
	return (0);
}
