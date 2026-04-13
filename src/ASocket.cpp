#include "ASocket.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <exception>
#include <fcntl.h>
#include <unistd.h>

ASocket::ASocket(void)
{
	this->socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketFd_ == -1)
		throw std::runtime_error("ASocket constructor failed, because socket() function failed.");

	fcntl(this->socketFd_, F_SETFL, O_NONBLOCK);
	if (this->socketFd_ == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->event.events = EPOLLIN;
	this->event.data.fd = this->socketFd_;
}

ASocket::ASocket(int socketFd):
socketFd__(socketFd)
{
	fcntl(this->socketFd_, F_SETFL, O_NONBLOCK);
	if (this->socketFd_ == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->event.events = EPOLLIN;
	this->event.data.fd = this->socketFd_;
}

ASocket::~ASocket(void)
{
	close(this->socketFd_);
}

int	ASocket::getFd(void)
{
	return (this->socketFd_);
}

struct epoll_event*	ASocket::getEvent(void)
{
	return (&this->event);
}
