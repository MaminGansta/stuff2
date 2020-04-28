
#include <stdio.h>
#pragma warning (disable: 4996)

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#define DEFAULT_BUFFER 4096

#define DEFAULT_COUNT       20
#define DEFAULT_PORT        5150
#define DEFAULT_BUFFER      2048


enum Packet
{
	P_InitConnection,
	P_ChatMessage,
	P_Test
};

SOCKET Connections[10];
int nConnections = 0;


bool ProccesPacket(int index, Packet packettype)
{
	switch (packettype)
	{
		case P_ChatMessage:
		{
			char msg[128];
			recv(Connections[index], msg, sizeof(msg), NULL);

			Packet packettype_send = P_ChatMessage;
			for (int i = 0; i < nConnections; i++)
			{
				if (index != i)
				{
					send(Connections[i], (char*)&packettype_send, sizeof(Packet), NULL);
					send(Connections[i], msg, sizeof(msg), NULL);
				}
			}
		}break;
		default:
			printf("unknow packet\n");
			return false;
	}
	return true;
}


void ClientHandler(int index)
{
	Packet packettype;
	while (true)
	{
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);

		if (!ProccesPacket(index, packettype))
			break;
	}
	closesocket(Connections[index]);
}


int main(void)
{

	WSADATA	wsd;
	char szBuffer[DEFAULT_BUFFER];

	int iPort = DEFAULT_PORT;  // Порт сервера
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


	for (int i = 0; i < 10; i++)
	{
		int server_size = sizeof(server);
		Connections[i] = accept(sListener, (SOCKADDR*)&server, &server_size);

		if (Connections[i] == 0)
		{
			printf("connection Error\n");
			i--;
			continue;
		}
		
		printf("New client connected\n");

		char msg[128] = "Hello, user";
		Packet packettype_send = P_ChatMessage;
		send(Connections[i], (char*)&packettype_send, sizeof(Packet), NULL);
		send(Connections[i], msg, sizeof(msg), NULL);

		nConnections++;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)i, NULL, NULL);
	}
	
	system("pause");
	return 0;
}