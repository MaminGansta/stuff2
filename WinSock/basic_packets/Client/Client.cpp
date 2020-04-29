
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
	P_Exit,
	P_Test
};

SOCKET Connection;
HANDLE server_callback;

// console callback
BOOL WINAPI console_callback(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	{
		// if console was closed, notify server and clear all stuff
		Packet packet_exit = P_Exit;
		send(Connection, (char*)&packet_exit, sizeof(Packet), NULL);

		TerminateThread(server_callback, 0);
		CloseHandle(server_callback);
		closesocket(Connection);
		WSACleanup();
		return TRUE;
	}

	default:
		return FALSE;
	}
}

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

		case P_Exit:
		{
			printf("Server shutdown\n");
			Connection = 0;
		}return false;

		default:
			printf("unkown packet\n");
			closesocket(Connection);
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
}


int main(void)
{
	// set console callback
	if (!SetConsoleCtrlHandler(console_callback, TRUE))
	{
		printf("\nERROR: Could not set control handler");
		return 1;
	}


	WSADATA	wsd;
	// load lib
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Failed to load Winsock library!\n");
		return 1;
	}


	SOCKADDR_IN client;
	hostent* host = NULL;

	client.sin_family = AF_INET;
	client.sin_port = htons(DEFAULT_PORT);
	client.sin_addr.s_addr = inet_addr("192.168.0.104");


	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&client, sizeof(client)))
	{
		printf("connection failled\n");
		return 1;
	}
	printf("Connection seccess\n");

	// create thread with receive msg hadnler
	server_callback = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	// send input text to the server
	char msg[128];
	Packet packettype_send = P_ChatMessage;

	while (true)
	{
		gets_s(msg);
		if (!Connection) break;

		if (strcmp("exit", msg) == 0)
		{
			Packet exit_packet = P_Exit;
			send(Connection, (char*)&exit_packet, sizeof(Packet), NULL);
			shutdown(Connection, SD_BOTH);
			break;
		}
		
		send(Connection, (char*)&packettype_send, sizeof(Packet), NULL);
		send(Connection, msg, sizeof(msg), NULL);

		Sleep(100);
	}

	CloseHandle(server_callback);
	closesocket(Connection);
	WSACleanup();
	return 0;
}