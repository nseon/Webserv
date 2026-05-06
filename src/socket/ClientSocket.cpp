#include "ClientSocket.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"
#include "Request.hpp"

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

#include <iostream> //tmp

#ifndef RECV_SIZE
# define RECV_SIZE 10
#endif

ClientSocket::ClientSocket(void) {}

ClientSocket::ClientSocket(int fd):
ASocket (fd) {}

ClientSocket::~ClientSocket(void) {}

int	ClientSocket::socketBehavior(void *pm)
{
	char	msg[RECV_SIZE];
	ssize_t	msg_length;

	if (this->_currentEvent & EPOLLRDHUP)
	{
		int		socketFd = this->_socketFd;
		reinterpret_cast<ServerManager*>(pm)->removeClientSocket(this->_socketFd);
		Logger::info() << "Client " << socketFd << " disconnected." << std::endl;
	}
	else
	{
		msg_length = recv(this->_socketFd, msg, RECV_SIZE, 0);
		msg[msg_length] = 0;
		Logger::info() << "Client " << this->_socketFd << " sended a message : " << msg << std::endl;
		try {
			_request.parseRequest(msg);
	
			if (_request.getParsingState() == DONE)
			{
				std::cout << _socketFd << ' ' << _request << std::flush;
				_request.reset();
			}
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
			_request.reset();
		}
	}
	return (0);
}
