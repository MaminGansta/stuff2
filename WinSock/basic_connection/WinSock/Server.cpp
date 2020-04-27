
#include <stdio.h>
#pragma warning (disable: 4996)

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#define DEFAULT_BUFFER 4096

#define DEFAULT_COUNT       20
#define DEFAULT_PORT        5150
#define DEFAULT_BUFFER      2048
#define DEFAULT_MESSAGE     "This is a test of the emergency broadcasting system"


int main(void)
{
	WSADATA	wsd;
	char		szBuffer[DEFAULT_BUFFER];

	int   iPort = DEFAULT_PORT;  // Порт сервера
	DWORD dwCount = DEFAULT_COUNT; // Сколько  отправить  
	

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Failed to load Winsock library!\n");
		return 1;
	}

	SOCKADDR_IN server;
	hostent* host = NULL;

	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");


	SOCKET sListener = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListener, (SOCKADDR*)&server, sizeof(server));
	listen(sListener, SOMAXCONN);


	SOCKET newConnetion;

	int server_size = sizeof(server);
	newConnetion = accept(sListener, (SOCKADDR*)&server, &server_size);

	if (newConnetion == 0)
	{
		printf("connection Error\n");
		return 1;
	}
	
	printf("connection seccess\n");
	char msg[128] = "Hello, new user";
	send(newConnetion, msg, sizeof(msg), NULL);

	system("pause");
	return 0;
}