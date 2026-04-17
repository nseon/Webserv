#include "ListenerSocket.hpp"
#include "ClientSocket.hpp"
#include "PollingManager.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <stdexcept>

#include <iostream>

#ifndef NULL
# define NULL (void*)0
#endif

ListenerSocket::ListenerSocket(unsigned short port)
{
	this->_address.sin_family = AF_INET;
	this->_address.sin_port = htons(port);
	this->_address.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&this->_address.sin_zero, 0, 8);

	if (bind(this->_socketFd, reinterpret_cast<struct sockaddr*>(&this->_address), sizeof(struct sockaddr)))
	{
		throw std::runtime_error("ListenerSocket constructor failed because bind() failed.");
	}

	if (listen(this->_socketFd, BACKLOG))
	{
		throw std::runtime_error("ListenerSocket constructor failed because listen() failed.");
	}
}

ListenerSocket::ListenerSocket(struct sockaddr_in address):
_address(address)
{
	if (bind(this->_socketFd, reinterpret_cast<struct sockaddr*>(&this->_address), sizeof(struct sockaddr)))
	{
		throw std::runtime_error("ListenerSocket constructor failed because bind() failed.");
	}

	if (listen(this->_socketFd, BACKLOG))
	{
		throw std::runtime_error("ListenerSocket constructor failed because listen() failed.");
	}
}

ListenerSocket::ListenerSocket(ListenerSocket const& toCopy):
ASocket(toCopy),
_address(toCopy._address) {}


ListenerSocket::~ListenerSocket(void) {}

int	ListenerSocket::socketBehavior(void* pm)
{
	int				newFd;

	newFd = accept(this->_socketFd, NULL, NULL);
	if (newFd == -1)
	{
		return (1);
	}
	reinterpret_cast<PollingManager*>(pm)->addClientSocket(newFd);
	std::cout << newFd << " joined the room." << std::endl;
	return (0);
}
