#include <iostream>
#include "ServerManager.hpp"
#include "ClientSocket.hpp"
#include "Logger.hpp"
#include "ASocket.hpp"

ServerManager::ServerManager(void) {}

ServerManager::ServerManager(std::vector<Server> servers):
_servers(servers)
{
	for (std::vector<Server>::iterator it = this->_servers.begin(); it < this->_servers.end(); it++)
	{
		it->createSocket();
		this->_pollingManager.addSocket(it->getSocket());
	}
	Logger::info() << "Server Manager is up !" << std::endl;
}

ServerManager::~ServerManager(void)
{
	for (std::vector<ClientSocket*>::iterator it = this->_clients.begin(); it < this->_clients.end(); it++)
	{
		delete *it;
	}
}

void	ServerManager::serverLoop(void)
{
	std::vector<ASocket*>	readyList;

	Logger::info() << "Server Manager begins to loop !" << std::endl;
	while (1)
	{
		readyList = this->_pollingManager.poll();

		for (std::vector<ASocket*>::iterator it = readyList.begin(); it < readyList.end(); it++)
		{
			(*it)->socketBehavior(this);
		}
	}
}

std::vector<ClientSocket*>::iterator	ServerManager::findClient(int socketFd)
{
	for (std::vector<ClientSocket*>::iterator it = this->_clients.begin(); it < this->_clients.end(); it++)
	{
		if ((*it)->getFd() == socketFd)
		{
			return (it);
		}
	}
	return (this->_clients.end());
}

void	ServerManager::addClientSocket(int socketFd)
{
	ClientSocket*	newCs = new ClientSocket(socketFd);

	this->_clients.push_back(newCs);
	this->_pollingManager.addSocket(newCs);
	Logger::info() << socketFd << "joined the room." << std::endl;
}

void	ServerManager::removeClientSocket(int socketFd)
{
	std::vector<ClientSocket*>::iterator	toRemove = this->findClient(socketFd);

	if (toRemove != this->_clients.end())
	{
		this->_pollingManager.removeSocket(socketFd);
		delete *toRemove;
		this->_clients.erase(toRemove);
	}
}
