
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#include <windows.h>
#include <stdio.h>
#include <utility>
#include <atomic>
#pragma warning (disable: 4996)


#define DEFAULT_BUFFER 4096

#define DEFAULT_COUNT       20
#define DEFAULT_PORT        5150
#define DEFAULT_BUFFER      2048


enum Packet
{
	P_InitConnection,
	P_ChatMessage,
	P_Exit,
	P_Test
};

bool wm_close = false;
HANDLE threads[10];
SOCKET Connections[10];
std::atomic_int nConnections = 0;


// console callback
BOOL WINAPI console_callback(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		case CTRL_CLOSE_EVENT:
			// if console was closed, notify clients and clear all stuff
			for (int i = 0; i < nConnections; i++)
			{
				Packet packet_exit = P_Exit;
				send(Connections[i], (char*)&packet_exit, sizeof(Packet), NULL);
			}

			for (int i = 0; i < nConnections; i++)
			{
				closesocket(Connections[i]);

				TerminateThread(threads[i], 0);
				CloseHandle(threads[i]);
			}
			WSACleanup();
			wm_close = true;
			return TRUE;

		default:
			return FALSE;
	}
}



bool ProccesPacket(int index, Packet packettype)
{
	printf("packettype: %d\n", packettype);

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

		case P_Exit:
		{
			std::swap(Connections[index], Connections[nConnections]);
			std::swap(threads[index], threads[nConnections]);

			closesocket(Connections[nConnections]);
			CloseHandle(threads[nConnections]);
			nConnections--;
		}return false;

		default:
			printf("unknow packet\n");
			closesocket(Connections[index]);
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
}


int main(void)
{
	// set controll callback funtion for this console window
	if (!SetConsoleCtrlHandler(console_callback, TRUE))
	{
		printf("\nERROR: Could not set control handler");
		return 1;
	}


	WSADATA	wsd;
	// start up the WSA lib
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Failed to load Winsock library!\n");
		return 1;
	}


	SOCKADDR_IN server;
	hostent* host = NULL;

	server.sin_family = AF_INET;
	server.sin_port = htons(DEFAULT_PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");


	SOCKET sListener = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListener, (SOCKADDR*)&server, sizeof(server));
	listen(sListener, SOMAXCONN);


	for (int i = 0; i < 10; i++)
	{
		int server_size = sizeof(server);
		Connections[nConnections] = accept(sListener, (SOCKADDR*)&server, &server_size);

		// if window was closed by x
		if (wm_close) break;

		if (Connections[nConnections] == 0)
		{
			printf("connection Error\n");
			i--;
			continue;
		}
		
		printf("New client connected\n");

		char msg[128] = "Hello, user";
		Packet packettype_send = P_ChatMessage;
		send(Connections[nConnections], (char*)&packettype_send, sizeof(Packet), NULL);
		send(Connections[nConnections], msg, sizeof(msg), NULL);

		int id = nConnections;
		threads[nConnections] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)id, NULL, NULL);
		nConnections++;
	}


	// clean up
	for (int i = 0; i < nConnections; i++)
	{
		Packet packet_close = P_Exit;
		send(Connections[i], (char*)&packet_close, sizeof(Packet), NULL);
		closesocket(Connections[i]);

		TerminateThread(threads[i], 0);
		CloseHandle(threads[i]);
	}
	
	WSACleanup();
	return 0;
}