#include <ctime>
#include <iostream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "manager/ServerManager.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/cgi.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/CGISocket.hpp"
#include "logger/Logger.hpp"
#include "socket/ASocket.hpp"

ServerManager::ServerManager(void) {}

ServerManager::ServerManager(std::vector<Server> servers):
_servers(servers),
_shouldStop(false)
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
		this->removeClientSocket((*it)->getFd());
	}
	for (std::vector<CGISocket*>::iterator it = this->_cgis.begin(); it < this->_cgis.end(); it++)
	{
		kill((*it)->getPid(), SIGKILL);
		waitpid((*it)->getPid(), NULL, 0);
		delete *it;
	}
}

void	ServerManager::serverLoop(void)
{
	std::vector<ASocket*>	readyList;

	Logger::info() << "Server Manager begins to loop !" << std::endl;
	while (!this->_shouldStop)
	{
		readyList = this->_pollingManager.poll(POLLING_TIMEOUT);

		for (std::vector<ASocket*>::iterator it = readyList.begin(); it < readyList.end(); it++)
		{
			(*it)->updateLastTimeUsed();
			(*it)->socketBehavior(this);
		}
		for (std::vector<CGISocket*>::iterator it = _cgis.begin(); it < _cgis.end(); it++)
		{
			if (difftime(std::time(NULL), (*it)->getLastTimeUsed()) >= CGI_TIMEOUT)
			{
				Response		response(*(*it)->getLocation(), (*it)->getVersion());
				ClientSocket*	client = (*it)->getClient();

				kill((*it)->getPid(), SIGKILL);
				waitpid((*it)->getPid(), NULL, 0);
				response.error(504, "Gateway Timeout");
				client->appendOutput(response.toString());
				this->enableClientWrite(client);
				this->removeCgiSocket(*it);
			}
		}
		for (std::vector<ClientSocket*>::iterator it = _clients.begin(); it < _clients.end(); it++)
		{
			if (difftime(std::time(NULL), (*it)->getLastTimeUsed()) >= CLIENT_TIMEOUT)
			{
				Logger::info() << "Client" << (*it)->getFd() << " timed out." << std::endl;
				this->removeClientSocket((*it)->getFd());
			}
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

	void	ServerManager::addClientSocket(int socketFd, struct sockaddr_in addr, Server* server)
	{
		ClientSocket*	newCs = new ClientSocket(socketFd, server, addr);

		this->_clients.push_back(newCs);
		this->_pollingManager.addSocket(newCs);
		Logger::info() << socketFd << " joined the room." << std::endl;
	}

	void	ServerManager::removeClientSocket(int socketFd)
	{
		std::vector<ClientSocket*>::iterator	toRemove = this->findClient(socketFd);

		if (toRemove != this->_clients.end())
		{
			ClientSocket*	client = *toRemove;

			for (std::vector<CGISocket*>::iterator it = this->_cgis.begin(); it < this->_cgis.end(); it++)
			{
				if ((*it)->getClient() == client)
				{
					kill((*it)->getPid(), SIGKILL);
					waitpid((*it)->getPid(), NULL, 0);
					this->removeCgiSocket(*it);
					break ;
				}
			}
			this->_requests.erase(client);
			this->_pollingManager.removeSocket(socketFd);
			delete client;
			this->_clients.erase(toRemove);
		}
	}

	void	ServerManager::enableClientWrite(ClientSocket* client)
	{
		client->enableWriteEvent();
		this->_pollingManager.modifySocket(client);
	}

	void	ServerManager::disableClientWrite(ClientSocket* client)
	{
		client->disableWriteEvent();
		this->_pollingManager.modifySocket(client);
	}

	void	ServerManager::modifyPolling(ASocket* socket)
	{
		this->_pollingManager.modifySocket(socket);
	}

	void	ServerManager::sendErrorResponse(ClientSocket* client, Location& location,
		std::string const& version, int code, std::string msg)
	{
		Response	response(location, version);

		response.error(code, msg);
		client->appendOutput(response.toString());
		this->enableClientWrite(client);
	}

	void	ServerManager::startCgi(ClientSocket* client, Request const& request,
		Location& location, CgiTarget const& target)
	{
		std::string	version = request.getVersion();
		std::string	method = request.getMethod();

		if ((method == "GET" && !location.getAllowGet())
			|| (method == "POST" && !location.getAllowPost()))
			return (this->sendErrorResponse(client, location, version, 405, "Method not allowed"));

		std::string	scriptPath = location.getRoot() + target.scriptName;
		int			execStatus = checkExecutable(scriptPath);

		if (execStatus == 404)
			return (this->sendErrorResponse(client, location, version, 404, "Not found"));
		if (execStatus == 403)
			return (this->sendErrorResponse(client, location, version, 403, "Forbidden"));

		int	sv[2];
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
			return (this->sendErrorResponse(client, location, version, 500, "Internal Server Error"));

		char**	envs = buildCgiEnvs(request, target, client->getAddress());
		pid_t	pid = fork();

		if (pid < 0)
		{
			close(sv[0]);
			close(sv[1]);
			freeCgiEnvs(envs);
			return (this->sendErrorResponse(client, location, version, 500, "Internal Server Error"));
		}
		if (pid == 0)
		{
			dup2(sv[1], STDIN_FILENO);
			dup2(sv[1], STDOUT_FILENO);
			close(sv[0]);
			close(sv[1]);

			size_t	slash = scriptPath.find_last_of('/');
			if (slash != std::string::npos && chdir(scriptPath.substr(0, slash).c_str()) != 0)
				this->setShouldStop();

			char*	argv[3];
			argv[0] = const_cast<char*>(target.interpreter.c_str());
			argv[1] = const_cast<char*>(scriptPath.c_str());
			argv[2] = NULL;
			execve(target.interpreter.c_str(), argv, envs);
			this->setShouldStop();
		}
		close(sv[1]);
		freeCgiEnvs(envs);

		CGISocket*	cgi = new CGISocket(sv[0], pid, client, request.getBody(), &location, version);

		this->_pollingManager.addSocket(cgi);
		this->_cgis.push_back(cgi);
		client->disableReadEvent();
		this->_pollingManager.modifySocket(client);
	}

	void	ServerManager::finalizeCgi(CGISocket* cgi)
	{
		pid_t	pid = cgi->getPid();
		int		status;

		if (waitpid(pid, &status, WNOHANG) == 0)
		{
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
		}

		ClientSocket*	client = cgi->getClient();
		bool			alive = false;

		for (std::vector<ClientSocket*>::iterator it = this->_clients.begin(); it < this->_clients.end(); it++)
			if (*it == client)
				alive = true;
		if (alive)
		{
		Response	response(*cgi->getLocation(), cgi->getVersion());

		if (!response.buildFromCgiOutput(cgi->getOutput()))
			response.error(502, "Bad Gateway");
		client->appendOutput(response.toString());
		client->enableReadEvent();
		client->enableWriteEvent();
		this->_pollingManager.modifySocket(client);
	}
	this->removeCgiSocket(cgi);
}

void	ServerManager::removeCgiSocket(CGISocket* cgi)
{
	this->_pollingManager.removeSocket(cgi->getFd());
	for (std::vector<CGISocket*>::iterator it = this->_cgis.begin(); it < this->_cgis.end(); it++)
	{
		if (*it == cgi)
		{
			this->_cgis.erase(it);
			break ;
		}
	}
	delete cgi;
}

void	ServerManager::handleHttpRequest(ClientSocket* client, char* msg)
{
	std::map<ClientSocket*, Request>::iterator	requestIterator;

	requestIterator = this->_requests.find(client);
	if (requestIterator == this->_requests.end())
	{
		this->_requests[client] = Request();
		requestIterator = this->_requests.find(client);
		requestIterator->second.setServer(requestIterator->first->getServer());
	}
	try
	{
		requestIterator->second.parseRequest(msg);

		if (requestIterator->second.getParsingState() == DONE)
		{
			Request&	request = requestIterator->second;
			Location*	location = request.getServer()->matchLocation(request.getPath());
			CgiTarget	target;

			if (location && parseCgiTarget(*location, request.getPath(), target))
				this->startCgi(client, request, *location, target);
			else
			{
				Response	response(request, location);

				client->appendOutput(response.toString());
				this->enableClientWrite(client);
			}
			request.reset();
		}
	}
	catch (int error_code)
	{
		Response	response(error_code, *requestIterator->first->getServer());

		client->appendOutput(response.toString());
		this->enableClientWrite(client);
		requestIterator->second.reset();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		requestIterator->second.reset();
	}
}

void	ServerManager::setShouldStop(void)
{
	this->_shouldStop = true;
}
