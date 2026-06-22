#include "socket/CGISocket.hpp"
#include "socket/ClientSocket.hpp"
#include "manager/ServerManager.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>

#ifndef CGI_READ_SIZE
# define CGI_READ_SIZE 40000
#endif

CGISocket::CGISocket(int fd, pid_t pid, ClientSocket* client,
	std::vector<char> const& body, Location* location, std::string const& version):
ASocket(fd, client->getServer()),
_pid(pid),
_client(client),
_location(location),
_version(version),
_toCgi(body),
_toCgi_off(0)
{
	if (!this->_toCgi.empty())
		this->enableWriteEvent();
}

CGISocket::~CGISocket(void) {}

pid_t	CGISocket::getPid(void) const
{
	return (this->_pid);
}

ClientSocket*	CGISocket::getClient(void) const
{
	return (this->_client);
}

Location*	CGISocket::getLocation(void) const
{
	return (this->_location);
}

std::string const&	CGISocket::getVersion(void) const
{
	return (this->_version);
}

std::vector<char> const&	CGISocket::getOutput(void) const
{
	return (this->_fromCgi);
}

int	CGISocket::socketBehavior(void* sm)
{
	ServerManager*	manager = reinterpret_cast<ServerManager*>(sm);

	if ((this->_currentEvent & EPOLLOUT) && this->_toCgi_off < this->_toCgi.size())
	{
		ssize_t	sent = send(this->_socketFd, &this->_toCgi[this->_toCgi_off],
			this->_toCgi.size() - this->_toCgi_off, MSG_NOSIGNAL);

		if (sent > 0)
		{
			this->_toCgi_off += static_cast<size_t>(sent);
			if (this->_toCgi_off >= this->_toCgi.size())
			{
				this->disableWriteEvent();
				manager->modifyPolling(this);
			}
		}
		else if (sent == 0)
		{
			this->disableWriteEvent();
			manager->modifyPolling(this);
		}
	}
	if (this->_currentEvent & (EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR))
	{
		char	buffer[CGI_READ_SIZE];
		bool	eof = false;

		while (true)
		{
			ssize_t	received = recv(this->_socketFd, buffer, CGI_READ_SIZE, 0);

			if (received > 0)
				this->_fromCgi.insert(this->_fromCgi.end(), buffer, buffer + received);
			else if (received == 0)
			{
				eof = true;
				break ;
			}
			else
				break ;
		}
		if (eof || (this->_currentEvent & (EPOLLHUP | EPOLLERR)))
		{
			manager->finalizeCgi(this);
			return (0);
		}
	}
	return (0);
}
