#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include <sys/epoll.h>
# include <netinet/in.h>
# include <string>

class Server;

class ASocket
{
	protected:
		int							_socketFd;
		struct epoll_event			_event;
		int							_currentEvent;
		struct sockaddr_in			_address;
		Server*						_server;

		ASocket(void);
	public:
		ASocket(Server* server);
		ASocket(Server* server, struct sockaddr_in address);
		ASocket(int socketFd, Server* server);
		ASocket(int socketFd, Server* server, struct sockaddr_in addr);
		ASocket(ASocket const& toCopy);
		virtual ~ASocket(void);

		int							getFd(void) const;
		int							getCurrentEvent(void) const;
		struct epoll_event const*	getEvent(void) const;
		Server*						getServer(void) const;
		struct epoll_event*			getNotConstEvent(void);
		std::string					getAddress(void) const;

		void						setCurrentEvent(int event);
		void						enableWriteEvent(void);
		void						disableWriteEvent(void);

		virtual int					socketBehavior(void *) = 0;
};

#endif
