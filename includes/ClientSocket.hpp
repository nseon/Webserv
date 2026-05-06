#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "ASocket.hpp"
# include "Request.hpp"

class ClientSocket: public ASocket
{
	private:
		ClientSocket(void);

	public:
		Request _request;
		
		ClientSocket(int fd);
		~ClientSocket(void);

		virtual int	socketBehavior(void *);
};

#endif
