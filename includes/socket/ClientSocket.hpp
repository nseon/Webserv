#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "socket/ASocket.hpp"
# include "http/Request.hpp"
# include "config/Server.hpp"
# include <vector>
# include <cstddef>
# include <string>

class ClientSocket: public ASocket
{
	private:
		ClientSocket(void);

		std::vector<char>	_out;
		size_t				_out_off;

	public:
		Request _request;

		ClientSocket(int fd, Server* server, struct sockaddr_in addr);
		~ClientSocket(void);

		void		appendOutput(std::string const& data);
		bool		hasPendingOutput(void) const;
		int			flush(void);

		virtual int	socketBehavior(void *);
};

#endif
