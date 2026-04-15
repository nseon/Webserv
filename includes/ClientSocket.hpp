#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "ASocket.hpp"

class ClientSocket: public ASocket
{
	private:
		ClientSocket(void);
	public:
		ClientSocket(int fd); 
		~ClientSocket(void);
		virtual ClientSocket*	clone(void) const;
		virtual int				socketBehavior(void *);
};

#endif
