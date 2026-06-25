#ifndef CGISOCKET_HPP
# define CGISOCKET_HPP

#include "http/Request.hpp"
# include "socket/ASocket.hpp"
# include "config/Location.hpp"
# include <vector>
# include <cstddef>
# include <string>
# include <sys/types.h>

class ClientSocket;

class CGISocket : public ASocket
{
	private:
		CGISocket(void);

		pid_t				_pid;
		ClientSocket*		_client;
		Location*			_location;
		Request const&			_request;
		std::string			_version;
		std::vector<char>	_toCgi;
		size_t				_toCgi_off;
		std::vector<char>	_fromCgi;

	public:
		CGISocket(int fd, pid_t pid, ClientSocket* client,
			Request const& request, Location* location, std::string const& version);
		~CGISocket(void);

		pid_t						getPid(void) const;
		ClientSocket*				getClient(void) const;
		Location*					getLocation(void) const;
		Request const&				getRequest(void) const;
		std::string const&			getVersion(void) const;
		std::vector<char> const&	getOutput(void) const;

		virtual int	socketBehavior(void *);
};

#endif
