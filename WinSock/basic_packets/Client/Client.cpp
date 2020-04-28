

#include <stdio.h>
#pragma warning (disable: 4996)

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#define DEFAULT_BUFFER 4096

#define DEFAULT_COUNT       20
#define DEFAULT_PORT        5150

enum Packet
{
	P_InitConnection,
	P_ChatMessage,
	P_Test
};

SOCKET Connection;


bool ProccesPacket(Packet packettype)
{
	switch (packettype)
	{
		case P_ChatMessage:
		{
			char msg[128];
			recv(Connection, msg, sizeof(msg), NULL);
			printf("%s\n", msg);
		}break;
		default:
			printf("unkown packet\n");
			return false;
	}
}


void ClientHandler()
{
	Packet packettype;
	while (true)
	{
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProccesPacket(packettype))
			break;
	}

	closesocket(Connection);
}


int main(void)
{
	WSADATA	wsd;
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


	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&client, sizeof(client)))
	{
		printf("connection failled\n");
		return 1;
	}
	printf("Connection seccess\n");

	// create thread with receive msg hadnler
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	// send input text to the server
	char msg[128];
	Packet packettype_send = P_ChatMessage;

	while (true)
	{
		gets_s(msg);
		send(Connection, (char*)&packettype_send, sizeof(Packet), NULL);
		send(Connection, msg, sizeof(msg), NULL);
		Sleep(10);
	}
	
	system("pause");
	return 0;
}