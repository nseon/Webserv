#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 3030

int	main(void)
{
	struct sockaddr_in	server;
	char	toSend[10] = "012345678";
	int		serverSocket;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&server.sin_zero, 0, 8);

	connect(serverSocket, reinterpret_cast<struct sockaddr *>(&server), static_cast<socklen_t>(sizeof(struct sockaddr)));

	send(serverSocket, toSend, 10, 0);
}
