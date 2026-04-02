#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstring>
#include <iostream>

#define PORT 3030



int	main(void)
{
	int					listeningSocket;
	int					newSocket;
	struct sockaddr_in	myAddr;
	struct sockaddr_in	sender;
	char				buf[10];
	int					size;
	int					epollFd;
	struct epoll_event	event;
	struct epoll_event	readyList[10];
	int					maxEvents;

	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(PORT);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&myAddr.sin_zero, 0, 8);

	bind(listeningSocket, reinterpret_cast<struct sockaddr *>(&myAddr), sizeof(struct sockaddr));
	listen(listeningSocket, 10);

	epollFd = epoll_create(1);
	event.events = EPOLLIN;
	event.data.fd = listeningSocket;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, listeningSocket, &event);
	maxEvents = 1;

	int readyEvents;
	while (1)
	{
		readyEvents = epoll_wait(epollFd,  readyList, maxEvents, -1);
		for (int i = 0; i < readyEvents; i++)
		{
			if (readyList[i].data.fd == listeningSocket)
			{
				newSocket = accept(listeningSocket, reinterpret_cast<struct sockaddr *>(&sender), reinterpret_cast<socklen_t *>(&size));
				epoll_ctl(epollFd, EPOLL_CTL_ADD, newSocket, &event);
			}
		}

		recv(newSocket, buf, 10, 0);

		std::cout << buf << std::endl;
	}
}
