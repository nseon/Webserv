#include "ListenerSocket.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>

ListenerSocket::ListenerSocket(int port)
{
	this->address.sin_family = AF_INET;
	this->address.sin_port = htons(port);
	this->address.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&this->address.sin_zero, 0, 8);

	if (bind(this->socketFd, reinterpret_cast<struct sockaddr*>(&this->address), sizeof(struct sockaddr)));
	{
		throw std::runtime_error("ListenerSocket constructor failed because bind() failed.");
	}

	if (listen(this->socketFd, BACKLOG))
	{
		throw std::runtime_error("ListenerSocket constructor failed because listen() failed.");
	}
}

ListenerSocket::~ListenerSocket(void) {}
