#include "ASocket.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>

ASocket::ASocket(void)
{
	this->_currentEvent = 0;
	this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because socket() function failed.");

	fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->_event.events = EPOLLIN;
	this->_event.data.fd = this->_socketFd;
}

ASocket::ASocket(int socketFd):
_socketFd(socketFd)
{
	this->_currentEvent = 0;
	fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->_event.events = EPOLLIN | EPOLLRDHUP;
	this->_event.data.fd = this->_socketFd;
}

ASocket::ASocket(ASocket const& toCopy):
_socketFd(toCopy._socketFd),
_event(toCopy._event),
_currentEvent(toCopy._currentEvent)
{}

ASocket::~ASocket(void)
{
	close(this->_socketFd);
}

int	ASocket::getFd(void) const
{
	return (this->_socketFd);
}

int	ASocket::getCurrentEvent(void) const
{
	return (this->_currentEvent);
}

struct epoll_event const*	ASocket::getEvent(void) const
{
	return (&this->_event);
}

struct epoll_event*	ASocket::getNotConstEvent(void)
{
	return (&this->_event);
}

void	ASocket::setCurrentEvent(int event)
{
	this->_currentEvent = event;
}
