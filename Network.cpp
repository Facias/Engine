#include "Network.h"
#include <stdio.h>

char *Network::getIP( char *hostname )
{
	char ip[100];
	struct hostent *he = NULL;
	struct in_addr **addr_list = NULL;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return "fail init";
	}

	printf("Initialised.\n");


	if ((he = gethostbyname(hostname)) == NULL)
	{
		//gethostbyname failed
		printf("gethostbyname failed : %d", WSAGetLastError());
		return "fail";
	}

	//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;

	for (int i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}

	printf("%s resolved to : %s\n", hostname, ip);

	

	return ip;


}

bool Network::bindSocket()
{

	SOCKET new_socket;
	struct sockaddr_in client;
	

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	int c = sizeof(struct sockaddr_in);
	while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted");

		//Reply to the client
		message = "Hello Client , I have received your connection. But I have to go now, bye\n";
		send(new_socket, message, strlen(message), 0);
	}

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return 1;
	}

	getchar();


	closesocket(s);
	WSACleanup();

	return true;
}