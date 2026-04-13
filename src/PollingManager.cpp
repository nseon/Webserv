#include "PollingManager.hpp"
#include <sys/epoll.h>
#include <exception>

PollingManager::PollingManager(void)
{
	this->epollInstance = epoll_create(1);
	if (this->epollInstance == -1)
	{
		throw std::runtime_error("PollingManager constructor failed, because epoll_create() failed.");
	}
}

void	PollingManager::addSocket(ASocket const& toAdd)
{
	if (epoll_ctl(this->epollInstance, EPOLL_CTL_ADD, toAdd.getFd, toAdd.getEvent))
	{
		throw std::runtime_error("PollingManager::addSocket failed because epoll_ctl() failed.");
	}
}

void	PollingManager::addSocket(ListenerSocket const& toAdd)
{
	this->addSocket(static_cast<ASocket>(toAdd));
	this->listenerSockets.push_back(toAdd.getFd);
}

void	PollingManager::addSocket(ClientSocket const& toAdd)
{
	this->addSocket(static_cast<ASocket>(toAdd));
	this->clientSockets.push_back(toAdd);
}
