#ifndef POLLINGMANAGER_HPP
# define POLLINGMANAGER_HPP

# include "ASocket.hpp"
# include <vector>

class PollingManager
{
	private:
		int						epollInstance;
		std::vector<int>		listenerSockets;
		std::vector<ASocket>	clientSockets;

	public:
		PollingManager(void);
		~PollingManager(void);

		void	addSocket(ASocket const& toAdd);
		void	addSocket(ListenerSocket const& toAdd);
		void	addSocket(ClientSocket const& toAdd);
};

#endif
