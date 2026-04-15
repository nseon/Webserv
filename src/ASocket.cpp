#include "ASocket.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdexcept>
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

	this->event_.events = EPOLLIN;
	this->event_.data.fd = this->socketFd_;
}

ASocket::ASocket(int socketFd):
socketFd_(socketFd)
{
	fcntl(this->socketFd_, F_SETFL, O_NONBLOCK);
	if (this->socketFd_ == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->event_.events = EPOLLIN;
	this->event_.data.fd = this->socketFd_;
}

ASocket::ASocket(ASocket const& toCopy):
socketFd_(toCopy.socketFd_),
event_(toCopy.event_) {}

ASocket::~ASocket(void)
{
	close(this->socketFd_);
}

int	ASocket::getFd(void) const
{
	return (this->socketFd_);
}

struct epoll_event const*	ASocket::getEvent(void) const
{
	return (&this->event_);
}

struct epoll_event*	ASocket::getNotConstEvent(void)
{
	return (&this->event_);
}
