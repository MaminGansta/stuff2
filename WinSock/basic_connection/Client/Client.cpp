

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

	SOCKADDR_IN client;
	hostent* host = NULL;

	client.sin_family = AF_INET;
	client.sin_port = htons(iPort);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");


	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&client, sizeof(client)))
	{
		printf("connection failled\n");
		return 1;
	}
	printf("Connection seccess\n");

	char msg[128];
	recv(Connection, msg, sizeof(msg), NULL);
	printf("%s\n", msg);
	
	system("pause");
	return 0;
}