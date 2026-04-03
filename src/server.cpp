#include <netinet/in.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstring>
#include <iostream>

#define PORT 3030
#define BACKLOG 10

static int	initServer(int* listeningSocket, struct sockaddr_in* serverAddr)
{
	*listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (*listeningSocket == -1)
	{
		return (1);
	}

	serverAddr->sin_family = AF_INET;
	serverAddr->sin_port = htons(PORT);
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&serverAddr->sin_zero, 0, 8);

	if (bind(*listeningSocket, reinterpret_cast<struct sockaddr*>(serverAddr), sizeof(struct sockaddr)))
	{
		close(*listeningSocket);
		return (1);
	}

	if (listen(*listeningSocket, BACKLOG))
	{
		close(*listeningSocket);
		return (1);
	}

	return (0);
}

static int	initEpoll(int* epollFd, int listeningSocket)
{
	struct epoll_event	event;

	*epollFd = epoll_create(1);
	if (*epollFd == -1)
	{
		return (1);
	}

	event.events = EPOLLIN;
	event.data.fd = listeningSocket;
	if (epoll_ctl(*epollFd, EPOLL_CTL_ADD, listeningSocket, &event))
	{
		close(*epollFd);
		return (1);
	}

	return (0);
}

static int	acceptNewSocket(int listeningSocket, struct epoll_event** events, int* current, int* max, int epollFd)
{
	int					newSocket;
	struct epoll_event	newEvent;
	struct epoll_event*	buf;

	newSocket = accept(listeningSocket, NULL, NULL);
	if (newSocket == -1)
	{
		return (1);
	}

	if (*current == *max)
	{
		buf = *events;
		*events = new struct epoll_event[*max * 2];
		*max *= 2;
		std::memcpy(*events, buf, sizeof(struct epoll_event) * static_cast<size_t>(*current));
		delete[] buf;
	}

	newEvent.data.fd = newSocket;
	newEvent.events = EPOLLIN;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, listeningSocket, &newEvent))
	{
		close(newSocket);
		return (1);
	}
	(*current)++;
	return (0);
}

static int	manageClient(int senderSocket)
{
	char	msg[1024];
	size_t	length = 1024;

	recv(senderSocket, msg, length, 0);
	std::cout << senderSocket << " : " << msg << std::endl;
	send(senderSocket, msg, 1024, 0);
	return (0);
}

static int	epollLoop(int epollFd, int listeningSocket)
{
	int					maxEvents = 1;
	int					readySockets;
	int					events_size = 10;
	struct epoll_event*	events;

	events = new struct epoll_event[events_size];
	while (1)
	{
		readySockets = epoll_wait(epollFd, events, maxEvents, -1);

		for (int i = 0; i < readySockets; i++)
		{
			if (events[i].data.fd == listeningSocket)
			{
				if (acceptNewSocket(listeningSocket, &events, &maxEvents, &events_size, epollFd))
				{
					return (1);
				}
			}
			else
			{
				manageClient(events[i].data.fd);
			}
		}
	}
}

int	main(void)
{
	int					ret;
	int					epollFd;
	int					listeningSocket;
	struct sockaddr_in	serverAddress;

	if (initServer(&listeningSocket, &serverAddress))
	{
		return (1);
	}

	if (initEpoll(&epollFd, listeningSocket))
	{
		close(listeningSocket);
		return (1);
	}

	ret = epollLoop(epollFd, listeningSocket);
	
	close (listeningSocket);
	return(ret);
}
