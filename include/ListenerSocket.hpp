#ifndef LISTENERSOCKET_HPP
# define LISTENERSOCKET_HPP

# include "ASocket.hpp"
# include "ClientSocket.hpp""
# include <sys/socket.h>

# ifndef BACKLOG
#  define BACKLOG 10
# endif

class ListenerSocket: public ASocket
{
	private:
		struct sockaddr_in	address;

		ListenerSocket(void);

	public:
		ListenerSocket(int port);
		~ListenerSocket(void);

		ClientSocket	acceptNewConnection(void);
};

#endif
