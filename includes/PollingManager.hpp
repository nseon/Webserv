#ifndef POLLINGMANAGER_HPP
# define POLLINGMANAGER_HPP

# include "ASocket.hpp"
#include <netinet/in.h>
# include <vector>

class PollingManager
{
	private:
		int						epollInstance_;
		std::vector<ASocket*>	sockets_;

		ASocket*	findSocket(int socketFd) const;
		void		addSocket(ASocket* toAdd);
	public:
		PollingManager(void);
		~PollingManager(void);

		void					addClientSocket(int socketFd);
		void					addListenerSocket(struct sockaddr_in address);
		void					removeSocket(int socketFd);
		std::vector<ASocket*>	poll(void) const;
};

#endif
