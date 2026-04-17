#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include <sys/epoll.h>

class ASocket
{
	protected:
		int							_socketFd;
		struct epoll_event			_event;

	public:
		ASocket(void);
		ASocket(int socketFd);
		ASocket(ASocket const& toCopy);
		virtual ~ASocket(void);

		int							getFd(void) const;
		struct epoll_event const*	getEvent(void) const;
		struct epoll_event*			getNotConstEvent(void);

		virtual int					socketBehavior(void *) = 0;
};

#endif
