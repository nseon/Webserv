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
	this->address_.sin_family = AF_INET;
	this->address_.sin_port = htons(port);
	this->address_.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&this->address_.sin_zero, 0, 8);

	if (bind(this->socketFd_, reinterpret_cast<struct sockaddr*>(&this->address_), sizeof(struct sockaddr)))
	{
		throw std::runtime_error("ListenerSocket constructor failed because bind() failed.");
	}

	if (listen(this->socketFd_, BACKLOG))
	{
		throw std::runtime_error("ListenerSocket constructor failed because listen() failed.");
	}
}

ListenerSocket::ListenerSocket(struct sockaddr_in address):
address_(address)
{
	if (bind(this->socketFd_, reinterpret_cast<struct sockaddr*>(&this->address_), sizeof(struct sockaddr)))
	{
		throw std::runtime_error("ListenerSocket constructor failed because bind() failed.");
	}

	if (listen(this->socketFd_, BACKLOG))
	{
		throw std::runtime_error("ListenerSocket constructor failed because listen() failed.");
	}
}

ListenerSocket::ListenerSocket(ListenerSocket const& toCopy):
ASocket(toCopy),
address_(toCopy.address_) {}


ListenerSocket::~ListenerSocket(void) {}

ListenerSocket*	ListenerSocket::clone(void) const
{
	return (new ListenerSocket(*this));
}

int	ListenerSocket::socketBehavior(void* pm)
{
	int				newFd;

	newFd = accept(this->socketFd_, NULL, NULL);
	if (newFd == -1)
	{
		return (1);
	}
	reinterpret_cast<PollingManager*>(pm)->addClientSocket(newFd);
	std::cout << newFd << " joined the room." << std::endl;
	return (0);
}
