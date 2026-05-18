#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include <sys/epoll.h>

class Server;

class ASocket
{
	protected:
		int							_socketFd;
		struct epoll_event			_event;
		int							_currentEvent;
		Server*						_server;

		ASocket(void);
	public:
		ASocket(Server* server);
		ASocket(int socketFd, Server* server);
		ASocket(ASocket const& toCopy);
		virtual ~ASocket(void);

		int							getFd(void) const;
		int							getCurrentEvent(void) const;
		struct epoll_event const*	getEvent(void) const;
		Server*						getServer(void) const;
		struct epoll_event*			getNotConstEvent(void);

		void						setCurrentEvent(int event);

		virtual int					socketBehavior(void *) = 0;
};

#endif
