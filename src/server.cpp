#include <netinet/in.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstring>
#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifndef NULL
# define NULL (void *)0 
#endif

#define RECV_SIZE 1024
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
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, newSocket, &newEvent))
	{
		close(newSocket);
		return (1);
	}
	(*current)++;
	return (0);
}

static int	sendMessage(int senderSocket, char* msg)
{
	std::cout << senderSocket << " : " << msg << std::flush;
	return (0);
}

static int	manageClient(int epollFd, int senderSocket)
{
	char	msg[RECV_SIZE];
	ssize_t	msg_length;

	msg_length = recv(senderSocket, msg, RECV_SIZE, 0);
	if (msg_length <= 0)
	{
		epoll_ctl(epollFd, EPOLL_CTL_DEL, senderSocket, NULL);
		close(senderSocket);
		std::strcpy(msg, "disconected.\n");
	}
	else
	{
		msg[msg_length] = 0;
	}
	sendMessage(senderSocket, msg);
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
					std::cout << errno << std::endl;
					perror(strerror(errno));
					return (1);
				}
				std::cout << "New Socket Accepted" << std::endl;
			}
			else
			{
				manageClient(epollFd, events[i].data.fd);
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
