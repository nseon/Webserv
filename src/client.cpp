#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <string>

#define PORT 3030

int	main(void)
{
	struct sockaddr_in	server;
	char				toSend[1024];
	std::string			line;
	int					serverSocket;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&server.sin_zero, 0, 8);

	connect(serverSocket, reinterpret_cast<struct sockaddr *>(&server), static_cast<socklen_t>(sizeof(struct sockaddr)));

	while (std::getline(std::cin, line))
	{
		strcpy(toSend, line.c_str());
		toSend << std::cin;
		send(serverSocket, toSend, 1024, 0);
	}
}
