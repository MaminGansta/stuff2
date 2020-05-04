
#pragma warning (disable: 4996)
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#include <windows.h>
#include <stdio.h>
#include <utility>
#include <atomic>
#include <shared_mutex>
#include <string>

#define MAX_CLIENTS 4


enum Packet
{
	P_Server_exit,

	// connection
	P_InitConnection,
	P_Exit,
	P_ServerFull,
	P_Test,

	// chat 
	P_ChatMessage,

	// game
	P_Map,
	P_Start,
	P_End,

	P_InitPosition,
	P_Player,
	P_Destroy
};

// game data
struct Bullet
{
	float pos_x = -1.0f, pos_y = -1.0f;
	float angle = 0;
	float speed_x = 0;
	float speed_y = 0;
	float speed = 0.6f;
};
struct Tank
{
	float pos_x = 0, pos_y = 0;
	int sprite = 0;

	Bullet bullet;
	float angle = 0;
	float speed = 0.2f;

	wchar_t name[16];
	int Score = 0;
	bool alive = true;
};
Tank players[4];

float init_pos[8] = { 0.03f, 0.03f,  0.9f, 0.9f,  0.9f, 0.03f,  0.03f, 0.9f };

int nDestroy = 0;
int destroy[4];

int map_size;
char* cur_map = NULL;

// flags
bool server_running = true;
bool game = false;
HANDLE hGame_thread;


// server
SOCKET sListener;
HANDLE threads[MAX_CLIENTS];

int nConnections = 0;
SOCKET Connections[MAX_CLIENTS];
std::shared_mutex mutex;



// Some complete send funtions
#include "messages.cpp"



/*
	Console callback. Catch console close event.
*/
BOOL WINAPI console_callback(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// if console was closed
	case CTRL_CLOSE_EVENT:
		sendCloseForAll();
		server_running = true;
		game = false;
		closesocket(sListener);
		return TRUE;

	default:
		return FALSE;
	}
}


/*
	When game is start. Begin send info to the players 
	in separate thread.

	Also in other threads recive info from players.
*/
void send_game_data()
{
	while (game)
	{
		Sleep(10);

		Packet packet = P_Player;
		for (int i = 0; i < nConnections; i++)
		{
			send(Connections[i], (char*)&packet, sizeof(Packet), NULL);

			int players_amount = nConnections - 1;
			send(Connections[i], (char*)&players_amount, sizeof(int), NULL);

			for (int j = 0; j < nConnections; j++)
			{
				if (i != j)
					send(Connections[i], (char*)&players[j], sizeof(Tank), NULL);
			}
		}

		packet = P_Destroy;
		if (nDestroy)
		{
			for (int i = 0; i < nConnections; i++)
			{
				send(Connections[i], (char*)&packet, sizeof(Packet), NULL);
				send(Connections[i], (char*)&nDestroy, sizeof(int), NULL);

				for (int j = 0; j < nDestroy; j++)
					send(Connections[i], (char*)&destroy[j], sizeof(int), NULL);
			}
			nDestroy = 0;
		}
	}

	printf("game thread done\n");
}



/*
	Handle clients messages.
*/
bool ProccesPacket(int index, Packet packettype)
{
	// if not game data print this message
	if (packettype != 10)
	printf("packettype: %d\n", packettype);

	switch (packettype)
	{
	
	// Init connetion
	case P_InitConnection:
	{

	}break;

	// Test packet
	case P_Test:
	{
		sendTest(index);
	}

	// Chat message
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


	/*
		Client want to disconnect.
	*/
	case P_Exit:
	{
		std::unique_lock<std::shared_mutex> lock(mutex);
		std::swap(Connections[index], Connections[nConnections]);
		std::swap(threads[index], threads[nConnections]);
		nConnections--;

		printf("client disconnect\n");
	}return false;


	/*
		Disconnect all clients, close send_game_info thread if it run.
		Close main socket and go to section clean up.

		(When host close his game or go out from lobby)
	*/
	case P_Server_exit:
	{
		server_running = false;
		game = false;
		sendCloseForAll();
		closesocket(sListener);

		printf("Server shudown\n");
	}return false;

	/*
		Recv map from host and send to the clients.
	*/
	case P_Map:
	{
		recv(Connections[index], (char*)&map_size, sizeof(int), NULL);

		free(cur_map);
		char* cur_map = (char*)malloc(map_size);

	
		int recived = 0;
		int left = map_size;
		int ind = 0;
		while (recived != map_size)
		{
			recived += recv(Connections[index], &cur_map[recived], left, NULL);
			left -= recived;
			ind += recived;
		}

		sendMap(index, cur_map, map_size);
	}break;

	/*
		Run game on clients machines
	*/
	case P_Start:
	{
		game = true;
		sendStart(index);
		//printf("Send start to %d", index);
		hGame_thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)send_game_data, NULL, NULL, NULL);
	}break;

	/*
		Recv player info and save it on server.
		Thean in game send info, send it to all
		players.
	*/
	case P_Player:
	{
		recv(Connections[index], (char*)&players[index], sizeof(Tank), NULL);
	}break;

	/*
		Recv destoyed obj info and store on server.
		Also in game send info, send it to all players.
	*/
	case P_Destroy:
	{
		int id;
		recv(Connections[index], (char*)&id, sizeof(int), NULL);
		destroy[nDestroy++] = id;
	}break;

	default:
		printf("unknow packet\n");
		closesocket(Connections[index]);
		return false;
	}
	return true;
}

/*
	Start in separe thread. And recv Clients messages
*/
void ClientHandler(int index)
{
	Packet packettype;
	while (server_running)
	{
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);

		if (!ProccesPacket(index, packettype))
			break;
	}

	printf("client %d disconnected\n", Connections[index]);
}


int main(int argc, const char* argv[])
{
	// set controll callback funtion for this console window
	if (!SetConsoleCtrlHandler(console_callback, TRUE))
	{
		printf("\nERROR: Could not set control handler");
		return 1;
	}

	// start up the WSA lib
	WSADATA	wsd;

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Failed to load Winsock library!\n");
		return 1;
	}


	SOCKADDR_IN server; 
	hostent* host = NULL;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[0]);
	server.sin_port = htons(atoi(argv[1]));

	//server.sin_addr.s_addr = inet_addr("192.168.0.104");
	//server.sin_port = htons(5678);


	sListener = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListener, (SOCKADDR*)&server, sizeof(server));
	listen(sListener, SOMAXCONN);

	printf("Server start up\n");
	printf("%s %s\n\n", argv[0], argv[1]);

	while (true)
	{
		int server_size = sizeof(server);
		SOCKET connection = accept(sListener, (SOCKADDR*)&server, &server_size);

		// if window was closed.
		if (!server_running) break;


		if (nConnections >= MAX_CLIENTS || game)
		{
			printf("server is full. Client %d cant connect\n", connection);
			sendServerFull(connection);
			shutdown(connection, CF_BOTH);
			continue;
		}

		if (connection == 0)
		{
			printf("connection Error\n");
			continue;
		}

		// take a lock (Same lock in delete client section)
		std::unique_lock<std::shared_mutex> lock(mutex);

		Connections[nConnections] = connection;
		printf("New client connected. Socket %d\n", connection);
		sendInit(nConnections);

		threads[nConnections] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)nConnections, NULL, NULL);
		nConnections++;
	}

	printf("server done\n");

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