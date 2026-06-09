#include <iostream>
#include <sys/socket.h>
#include "manager/ServerManager.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "socket/ClientSocket.hpp"
#include "logger/Logger.hpp"
#include "socket/ASocket.hpp"

ServerManager::ServerManager(void) {}

ServerManager::ServerManager(std::vector<Server> servers):
_servers(servers)
{
	for (std::vector<Server>::iterator it = this->_servers.begin(); it < this->_servers.end(); it++)
	{
		try
		{
			it->createSocket();
		}
		catch (std::exception &e)
		{
			std::cout << "Manager failed to create because :" << e.what() << std::endl;
			return ;
		}
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

void	ServerManager::addClientSocket(int socketFd, Server* server)
{
	ClientSocket*	newCs = new ClientSocket(socketFd, server);

	this->_clients.push_back(newCs);
	this->_pollingManager.addSocket(newCs);
	Logger::info() << socketFd << " joined the room." << std::endl;
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

void	ServerManager::handleHttpRequest(ClientSocket* client, std::string &msg)
{
	std::map<ClientSocket*, Request>::iterator	requestIterator;

	requestIterator = this->_requests.find(client);
	if (requestIterator == this->_requests.end())
	{
		this->_requests[client] = Request();
		requestIterator = this->_requests.find(client);
		requestIterator->second.setServer(requestIterator->first->getServer());
	}
	try {
		requestIterator->second.parseRequest(msg);
		if (requestIterator->second.getParsingState() == DONE)
		{
			Response response(requestIterator->second, requestIterator->first->getServer()->matchLocation(requestIterator->second.getPath()));
			std::string response_str = response.toString();
	
			send(client->getFd(), response_str.c_str(), response_str.size(), 0);
			requestIterator->second.reset();
		}
	}
	catch (int error_code)
	{
		Response response(error_code, *requestIterator->first->getServer());
		std::string response_str = response.toString();

		send(client->getFd(), response_str.c_str(), response_str.size(), 0);
		requestIterator->second.reset();
	}
}
