#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include <sys/epoll.h>

class ASocket
{
	protected:
		int	socketFd_;
		struct epoll_event	event;

	public:
		ASocket(void);
		ASocket(int socketFd);
		~ASocket(void);

		int	getFd(void);
};

#endif
