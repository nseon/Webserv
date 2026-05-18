#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "socket/ASocket.hpp"
# include "http/Request.hpp"
# include "config/Server.hpp"

class ClientSocket: public ASocket
{
	private:
		ClientSocket(void);

	public:
		Request _request;
		
		ClientSocket(int fd, Server* server);
		~ClientSocket(void);

		virtual int	socketBehavior(void *);
};

#endif
