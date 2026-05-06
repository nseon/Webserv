#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <vector>
# include "socket/ClientSocket.hpp"
# include "polling/PollingManager.hpp"
# include "config/Server.hpp"

class ServerManager
{
	private:
		ServerManager(void);

		std::vector<Server>						_servers;
		std::vector<ClientSocket*>				_clients;
		PollingManager							_pollingManager;

		std::vector<ClientSocket*>::iterator	findClient(int socketFd);

	public:
		ServerManager(std::vector<Server> servers);
		~ServerManager(void);

		void	serverLoop(void);

		void	addClientSocket(int socketFd);
		void	removeClientSocket(int socketFd);
};

#endif
