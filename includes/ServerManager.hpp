#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <vector>
# include "PollingManager.hpp"
# include "Server.hpp"

class ServerManager
{
	private:
		std::vector<Server>	_servers;
		PollingManager		_pollingManager;

	public:
		ServerManager(void);
		ServerManager(std::vector<Server> servers);

		void	serverLoop(void);
};

#endif
