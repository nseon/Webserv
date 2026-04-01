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

	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(PORT);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&myAddr.sin_zero, 0, 8);

	bind(listeningSocket, reinterpret_cast<struct sockaddr *>(&myAddr), sizeof(struct sockaddr));
	listen(listeningSocket, 10);

	epollFd = epoll_create(1);
	epoll_ctl(epollFd, EPOLL_CTL_ADD, listeningSocket, );

	while (1)
	{
		newSocket = accept(listeningSocket, reinterpret_cast<struct sockaddr *>(&sender), reinterpret_cast<socklen_t *>(&size));

		recv(newSocket, buf, 10, 0);

		std::cout << buf << std::endl;
	}
}
