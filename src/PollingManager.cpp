#include "PollingManager.hpp"
#include "ASocket.hpp"
#include "ClientSocket.hpp"
#include "ListenerSocket.hpp"
#include <netinet/in.h>
#include <sys/epoll.h>
#include <stdexcept>

PollingManager::PollingManager(void)
{
	this->epollInstance_ = epoll_create(1);
	if (this->epollInstance_ == -1)
	{
		throw std::runtime_error("PollingManager constructor failed, because epoll_create() failed.");
	}
}

PollingManager::~PollingManager(void)
{
	for (std::vector<ASocket*>::iterator it = this->sockets_.begin(); it < this->sockets_.end(); it++)
	{
		delete *it;
	}
}

void	PollingManager::addSocket(ASocket* toAdd)
{
	this->sockets_.push_back(toAdd);
	if (epoll_ctl(this->epollInstance_, EPOLL_CTL_ADD, toAdd->getFd(), toAdd->getNotConstEvent()))
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
	epoll_ctl(this->epollInstance_, EPOLL_CTL_DEL, socketFd, (*toDel)->getNotConstEvent());
	delete *toDel;
	this->sockets_.erase(toDel);
}

std::vector<ASocket*>::iterator	PollingManager::findSocket(int socketFd)
{
	for (std::vector<ASocket*>::iterator it = this->sockets_.begin(); it < this->sockets_.end(); it++)
	{
		if (socketFd == (*it)->getFd())
		{
			return (it);
		}
	}
	return (this->sockets_.end());
}

std::vector<ASocket*>	PollingManager::poll(void)
{
	struct epoll_event*		events = new struct epoll_event[this->sockets_.size()];
	std::vector<ASocket*>	ret;
	int						nbEvents;

	nbEvents = epoll_wait(this->epollInstance_, events, static_cast<int>(this->sockets_.size()), -1);
	for (int i = 0; i < nbEvents; i++)
	{
		ret.push_back(*this->findSocket(events[i].data.fd));
	}
	delete[] events;
	return (ret);
}
