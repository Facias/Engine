#ifndef NETWORK_H_
#define NETWORK_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")


struct Network
{
	Network()
	{
		
	}


	char *getIP(char *hostname);
	bool bindSocket();

	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;

	

	char *message, server_reply[2000];
	int recv_size;
};


#endif