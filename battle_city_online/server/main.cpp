
#pragma warning (disable: 4996)
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#include <windows.h>
#include <stdio.h>
#include <utility>
#include <atomic>
#include <shared_mutex>
#include <string>

#define DEFAULT_PORT	5678

enum Packet
{
	// connection
	P_InitConnection,
	P_ServerFull,
	P_Exit,
	P_Test,

	// chat 
	P_ChatMessage


	// game
};

bool wm_close = false;
HANDLE threads[10];
SOCKET Connections[10];
std::atomic_int nConnections = 0;
std::shared_mutex mutex;


#include "messages.cpp"


// console callback
BOOL WINAPI console_callback(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// if console was closed
	case CTRL_CLOSE_EVENT:
		for (int i = 0; i < nConnections; i++)
		{
			// notify clients for the end of connection
			Packet packet_exit = P_Exit;
			send(Connections[i], (char*)&packet_exit, sizeof(Packet), NULL);
		}

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

		// if add new client (to the array) or disconect client (remove from the array)
		std::shared_lock<std::shared_mutex> lock(mutex);

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
		std::unique_lock<std::shared_mutex> lock(mutex);
		std::swap(Connections[index], Connections[nConnections]);
		std::swap(threads[index], threads[nConnections]);

		closesocket(Connections[nConnections]);
		CloseHandle(threads[nConnections]);
		nConnections--;
		printf("client disconnect\n");
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
	server.sin_addr.s_addr = inet_addr("192.168.0.104");


	SOCKET sListener = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListener, (SOCKADDR*)&server, sizeof(server));
	listen(sListener, SOMAXCONN);


	while (true)
	{
		int server_size = sizeof(server);
		SOCKET connection = accept(sListener, (SOCKADDR*)&server, &server_size);

		if (nConnections > 5)
		{
			Packet packet_exit = P_ServerFull;
			send(connection, (char*)&packet_exit, sizeof(Packet), NULL);
			shutdown(connection, CF_BOTH);
			continue;
		}

		std::unique_lock<std::shared_mutex> lock(mutex);
		Connections[nConnections] = connection;

		// if window was closed by x
		if (wm_close) break;

		if (Connections[nConnections] == 0)
		{
			printf("connection Error\n");
			continue;
		}

		printf("New client connected. Socket %d\n", connection);
		sendInit(nConnections);

		int id = nConnections;
		threads[nConnections] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)id, NULL, NULL);
		nConnections++;
	}


	// clean up
	for (int i = 0; i < nConnections; i++)
	{
		shutdown(Connections[i], SD_BOTH);
		closesocket(Connections[i]);
		TerminateThread(threads[i], 0);
		CloseHandle(threads[i]);
	}

	WSACleanup();
	return 0;
}