#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <vector>
# include <map>
# include "http/Request.hpp"
# include "socket/ClientSocket.hpp"
# include "polling/PollingManager.hpp"
# include "config/Server.hpp"

class ServerManager
{
	private:
		ServerManager(void);

		std::vector<Server>						_servers;
		std::vector<ClientSocket*>				_clients;
		std::map<ClientSocket*, Request>		_requests;
		PollingManager							_pollingManager;

		std::vector<ClientSocket*>::iterator	findClient(int socketFd);

	public:
		ServerManager(std::vector<Server> servers);
		~ServerManager(void);

		void	serverLoop(void);

		void	addClientSocket(int socketFd);
		void	removeClientSocket(int socketFd);

		void	handleHttpRequest(ClientSocket* client, char* msg);
};

#endif
