#include "socket/ASocket.hpp"
#include <netinet/in.h>
#include <sstream>
#include <cstring>
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>

ASocket::ASocket(Server* server):
_server(server)
{
	std::time(&this->_lastTimeUsed);
	this->_currentEvent = 0;
	this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because socket() function failed.");

	fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
	fcntl(this->_socketFd, F_SETFD, FD_CLOEXEC);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->_event.events = EPOLLIN;
	this->_event.data.fd = this->_socketFd;
	std::memset(&this->_address, 0, sizeof(this->_address));
}

ASocket::ASocket(Server* server, struct sockaddr_in address):
_address(address),
_server(server)
{
	std::time(&this->_lastTimeUsed);
	this->_currentEvent = 0;
	this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because socket() function failed.");

	fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
	fcntl(this->_socketFd, F_SETFD, FD_CLOEXEC);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->_event.events = EPOLLIN;
	this->_event.data.fd = this->_socketFd;
}

ASocket::ASocket(int socketFd, Server* server):
_socketFd(socketFd),
_server(server)
{
	std::time(&this->_lastTimeUsed);
	this->_currentEvent = 0;
	fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
	fcntl(this->_socketFd, F_SETFD, FD_CLOEXEC);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->_event.events = EPOLLIN | EPOLLRDHUP;
	this->_event.data.fd = this->_socketFd;
	std::memset(&this->_address, 0, sizeof(this->_address));
}

ASocket::ASocket(int socketFd, Server* server, struct sockaddr_in addr):
_socketFd(socketFd),
_address(addr),
_server(server)
{
	std::time(&this->_lastTimeUsed);
	this->_currentEvent = 0;
	fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
	fcntl(this->_socketFd, F_SETFD, FD_CLOEXEC);
	if (this->_socketFd == -1)
		throw std::runtime_error("ASocket constructor failed, because fnctl() function failed.");

	this->_event.events = EPOLLIN | EPOLLRDHUP;
	this->_event.data.fd = this->_socketFd;
}

ASocket::ASocket(ASocket const& toCopy):
_socketFd(toCopy._socketFd),
_event(toCopy._event),
_currentEvent(toCopy._currentEvent),
_address(toCopy._address),
_server(toCopy._server),
_lastTimeUsed(toCopy._lastTimeUsed)
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

Server*	ASocket::getServer(void) const
{
	return (this->_server);
}

struct epoll_event*	ASocket::getNotConstEvent(void)
{
	return (&this->_event);
}

std::string	ASocket::getAddress(void) const
{
	uint32_t			addr= ntohl(this->_address.sin_addr.s_addr);
	std::string			ret;

	for (int i = 24; i >= 0; i -= 8)
	{
		std::stringstream	ss;
		ss << ((addr >> i) & 0xFF);
		ret += ss.str();
		if (i)
		{
			ret += '.';
		}
	}
	return (ret);
}

time_t	ASocket::getLastTimeUsed(void) const
{
	return (this->_lastTimeUsed);
}

void	ASocket::setCurrentEvent(int event)
{
	this->_currentEvent = event;
}

void	ASocket::enableWriteEvent(void)
{
	this->_event.events |= EPOLLOUT;
}

void	ASocket::disableWriteEvent(void)
{
	this->_event.events &= ~EPOLLOUT;
}

void	ASocket::enableReadEvent(void)
{
	this->_event.events |= EPOLLIN;
}

void	ASocket::disableReadEvent(void)
{
	this->_event.events &= ~EPOLLIN;
}

void	ASocket::updateLastTimeUsed(void)
{
	std::time(&this->_lastTimeUsed);
}
