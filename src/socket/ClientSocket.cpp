#include "socket/ClientSocket.hpp"
#include "manager/ServerManager.hpp"
#include "logger/Logger.hpp"
#include "http/Request.hpp"

#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

#include <iostream> //tmp

#ifndef RECV_SIZE
# define RECV_SIZE 1024
#endif

ClientSocket::ClientSocket(int fd, Server* server, struct sockaddr_in addr):
ASocket (fd, server, addr), _out_off(0) {}

ClientSocket::~ClientSocket(void) {}

void	ClientSocket::appendOutput(std::string const& data)
{
	this->_out.insert(this->_out.end(), data.begin(), data.end());
}

bool	ClientSocket::hasPendingOutput(void) const
{
	return (this->_out_off < this->_out.size());
}

int	ClientSocket::flush(void)
{
	ssize_t	sent;

	if (!this->hasPendingOutput())
		return (1);
	sent = send(this->_socketFd, &this->_out[this->_out_off],
		this->_out.size() - this->_out_off, MSG_NOSIGNAL);
	if (sent <= 0)
		return (-1);
	this->_out_off += static_cast<size_t>(sent);
	if (this->_out_off >= this->_out.size())
	{
		this->_out.clear();
		this->_out_off = 0;
		return (1);
	}
	return (0);
}

int	ClientSocket::socketBehavior(void *sm)
{
	ServerManager*	manager = reinterpret_cast<ServerManager*>(sm);
	char			msg[RECV_SIZE + 1];
	ssize_t			msg_length;

	if (this->_currentEvent & EPOLLRDHUP)
	{
		int	socketFd = this->_socketFd;
		manager->removeClientSocket(this->_socketFd);
		Logger::info() << "Client " << socketFd << " disconnected." << std::endl;
		return (0);
	}
	if (this->_currentEvent & EPOLLIN)
	{
		msg_length = recv(this->_socketFd, msg, RECV_SIZE, 0);
		if (msg_length <= 0)
		{
			manager->removeClientSocket(this->_socketFd);
			return (0);
		}
		msg[msg_length] = 0;
		Logger::info() << "Client " << this->_socketFd << " sended a message : " << msg << std::endl;
		manager->handleHttpRequest(this, msg);
	}
	if (this->_currentEvent & EPOLLOUT)
	{
		int	state = this->flush();

		if (state == 1)
			manager->disableClientWrite(this);
		else if (state == -1)
		{
			manager->removeClientSocket(this->_socketFd);
			return (0);
		}
	}
	return (0);
}
