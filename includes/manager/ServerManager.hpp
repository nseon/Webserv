#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <vector>
# include <map>
# include "http/Request.hpp"
# include "socket/ClientSocket.hpp"
# include "polling/PollingManager.hpp"
# include "config/Server.hpp"

# ifndef POLLING_TIMEOUT
#  define POLLING_TIMEOUT 60000
# endif

# ifndef CLIENT_TIMEOUT
#  define CLIENT_TIMEOUT 60
# endif

# ifndef CGI_TIMEOUT
#  define CGI_TIMEOUT 60
# endif

class CGISocket;
struct CgiTarget;

class ServerManager
{
	private:
		ServerManager(void);

		std::vector<Server>						_servers;
		std::vector<ClientSocket*>				_clients;
		std::map<ClientSocket*, Request>		_requests;
		std::vector<CGISocket*>					_cgis;
		PollingManager							_pollingManager;

		std::vector<ClientSocket*>::iterator	findClient(int socketFd);
		void									sendErrorResponse(ClientSocket* client, Location& location,
													std::string const& version, int code);
		bool									_shouldStop;

	public:
		ServerManager(std::vector<Server> servers);
		~ServerManager(void);

		void	serverLoop(void);

		void	addClientSocket(int socketFd, struct sockaddr_in addr, Server* server);
		void	removeClientSocket(int socketFd);

		void	enableClientWrite(ClientSocket* client);
		void	disableClientWrite(ClientSocket* client);
		void	modifyPolling(ASocket* socket);

		void	startCgi(ClientSocket* client, Request const& request, Location& location, CgiTarget const& target);
		void	finalizeCgi(CGISocket* cgi);
		void	removeCgiSocket(CGISocket* cgi);

		void	handleHttpRequest(ClientSocket* client, std::string &msg);

		void	setShouldStop(void);
};

#endif
