#include "ServerManager.hpp"
#include "Logger.hpp"
#include "ASocket.hpp"

ServerManager::ServerManager(void) {}

ServerManager::ServerManager(std::vector<Server> servers):
_servers(servers)
{
	for (std::vector<Server>::iterator it = this->_servers.begin(); it < this->_servers.end(); it++)
	{
		this->_pollingManager.addSocket(it->getSocket());
	}
	Logger::info() << "Server Manager is up !" << std::endl;
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
			(*it)->socketBehavior();
		}
	}
}

void	ServerManager::addClientSocket(int socketFd)
void	ServerManager::removeClientSocket(int socketFd)
