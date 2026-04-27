#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <vector>
# include "ClientSocket.hpp"
# include "PollingManager.hpp"
# include "Server.hpp"

class ServerManager
{
	private:
		ServerManager(void);

		std::vector<Server>			_servers;
		std::vector<ClientSocket>	_clients;
		PollingManager				_pollingManager;

	public:
		ServerManager(std::vector<Server> servers);

		void	serverLoop(void);

		void	addClientSocket(int socketFd);
		void	removeClientSocket(int socketFd);
};

#endif
