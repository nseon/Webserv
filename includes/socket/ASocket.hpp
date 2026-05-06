#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include <sys/epoll.h>

class ASocket
{
	protected:
		int							_socketFd;
		struct epoll_event			_event;
		int							_currentEvent;

	public:
		ASocket(void);
		ASocket(int socketFd);
		ASocket(ASocket const& toCopy);
		virtual ~ASocket(void);

		int							getFd(void) const;
		int							getCurrentEvent(void) const;
		struct epoll_event const*	getEvent(void) const;
		struct epoll_event*			getNotConstEvent(void);

		void						setCurrentEvent(int event);

		virtual int					socketBehavior(void *) = 0;
};

#endif
