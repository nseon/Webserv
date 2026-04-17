#include "PollingManager.hpp"
#include "ASocket.hpp"
#include "ClientSocket.hpp"
#include "ListenerSocket.hpp"
#include <netinet/in.h>
#include <sys/epoll.h>
#include <stdexcept>

PollingManager::PollingManager(void)
{
	this->_epollInstance = epoll_create(1);
	if (this->_epollInstance == -1)
	{
		throw std::runtime_error("PollingManager constructor failed, because epoll_create() failed.");
	}
}

PollingManager::~PollingManager(void)
{
	for (std::vector<ASocket*>::iterator it = this->_sockets.begin(); it < this->_sockets.end(); it++)
	{
		delete *it;
	}
}

void	PollingManager::addSocket(ASocket* toAdd)
{
	this->_sockets.push_back(toAdd);
	if (epoll_ctl(this->_epollInstance, EPOLL_CTL_ADD, toAdd->getFd(), toAdd->getNotConstEvent()))
	{
		throw std::runtime_error("PollingManager::addSocket failed because epoll_ctl() failed.");
	}
}

void	PollingManager::addClientSocket(int socketFd)
{
	ClientSocket*	newCs = new ClientSocket(socketFd);
	this->addSocket(newCs);
}

void	PollingManager::addListenerSocket(struct sockaddr_in address)
{
	ListenerSocket*	newLs = new ListenerSocket(address);
	this->addSocket(newLs);
}

void	PollingManager::removeSocket(int socketFd)
{
	std::vector<ASocket*>::iterator	toDel = this->findSocket(socketFd);
	epoll_ctl(this->_epollInstance, EPOLL_CTL_DEL, socketFd, (*toDel)->getNotConstEvent());
	delete *toDel;
	this->_sockets.erase(toDel);
}

std::vector<ASocket*>::iterator	PollingManager::findSocket(int socketFd)
{
	for (std::vector<ASocket*>::iterator it = this->_sockets.begin(); it < this->_sockets.end(); it++)
	{
		if (socketFd == (*it)->getFd())
		{
			return (it);
		}
	}
	return (this->_sockets.end());
}

std::vector<ASocket*>	PollingManager::poll(void)
{
	struct epoll_event*		events = new struct epoll_event[this->_sockets.size()];
	int						nbEvents;

	nbEvents = epoll_wait(this->_epollInstance, events, static_cast<int>(this->_sockets.size()), -1);
	if (nbEvents == -1) // TODO
	{

	}
	std::vector<ASocket*>	ret(static_cast<unsigned long>(nbEvents));
	for (int i = 0; i < nbEvents; i++)
	{
		ret.push_back(*this->findSocket(events[i].data.fd));
	}
	delete[] events;
	return (ret);
}
