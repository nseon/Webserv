#include "PollingManager.hpp"
#include <netinet/in.h>
#include <cstring>

#ifndef PORT
# define PORT 3030
#endif

int	main(void)
{
	struct sockaddr_in		address;
	PollingManager			pm;
	std::vector<ASocket*>	readyList;

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(&address.sin_zero, 0, 8);

	pm.addListenerSocket(address);
	while (1)
	{
		readyList = pm.poll();
		for (unsigned int i = 0; i < readyList.size(); i++)
		{
			readyList[i]->socketBehavior(&pm);
		}
	}
}
