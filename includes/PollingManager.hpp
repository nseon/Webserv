#ifndef POLLINGMANAGER_HPP
# define POLLINGMANAGER_HPP

# include "ASocket.hpp"
# include <netinet/in.h>
# include <vector>

class PollingManager
{
	private:
		int								_epollInstance;
		std::vector<ASocket*>			_sockets;

		std::vector<ASocket*>::iterator	findSocket(int socketFd);

	public:
		PollingManager(void);
		~PollingManager(void);

		void							addSocket(ASocket* toAdd);
		void							removeSocket(int socketFd);
		std::vector<ASocket*>			poll(void);
};

#endif
