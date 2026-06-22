#ifndef LISTENERSOCKET_HPP
# define LISTENERSOCKET_HPP

# include "socket/ASocket.hpp"
# include "config/Server.hpp"
# include <netinet/in.h>

# ifndef BACKLOG
#  define BACKLOG 10
# endif

class Server;

class ListenerSocket: public ASocket
{
	private:
		ListenerSocket(void);

	public:
		ListenerSocket(unsigned short port);
		ListenerSocket(struct sockaddr_in address, Server* server);
		ListenerSocket(ListenerSocket const& toCopy);
		~ListenerSocket(void);

		virtual int			socketBehavior(void* sm);
};

#endif
