
#pragma warning (disable: 4996)
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#include <windows.h>
#include <stdio.h>
#include <utility>
#include <atomic>
#include <shared_mutex>
#include <string>
#include <unordered_map>


void recv_s(SOCKET sock, char* buf, int size, int flags = 0)
{
	int recived = 0;
	int left = size;
	int ind = 0;
	while (left)
	{
		recived = recv(sock, &buf[ind], left, flags);
		left -= recived;
		ind += recived;
	}
}



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
	P_Enemies,
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
//Tank players[4];
std::unordered_map<SOCKET, Tank> players;

float init_pos[8] = { 0.03f, 0.03f,  0.97f, 0.97f,  0.97f, 0.03f,  0.03f, 0.97f };

int nDestroy = 0;
int destroy[4];

int map_size = 0;
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
		sendCloseForAll(100);
		server_running = true;
		game = false;
		closesocket(sListener);
		return TRUE;

	default:
		return FALSE;
	}
}


// ===================================== Send game data ===============================================

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

		// if add new client (to the array) or disconect client (remove from the array)
		std::shared_lock<std::shared_mutex> lock(mutex);

		// Send player status
		Packet packet = P_Player;
		for (int i = 0; i < nConnections; i++)
		{
			send(Connections[i], (char*)&packet, sizeof(Packet), NULL);
			send(Connections[i], (char*)&players[Connections[i]], sizeof(Tank), NULL);
		}

		// Send Enemy status
		packet = P_Enemies;
		for (int i = 0; i < nConnections; i++)
		{
			send(Connections[i], (char*)&packet, sizeof(Packet), NULL);

			int enemies_amount = nConnections - 1;
			send(Connections[i], (char*)&enemies_amount, sizeof(int), NULL);

			for (int j = 0; j < nConnections; j++)
			{
				if (i != j)
				send(Connections[i], (char*)&players[Connections[j]], sizeof(Tank), NULL);
			}
		}

		// Send distoyed objects info
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





// ======================================== Client callback ====================================================
/*
	Handle clients messages.
*/
bool ProccesPacket(SOCKET connection, Packet packettype)
{
	// if not game data print this message
	if (packettype < 10)
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
		sendTest(connection);
	}

	// Chat message
	case P_ChatMessage:
	{
	}break;


	/*
		Client want to disconnect.
	*/
	case P_Exit:
	{
		printf("client: %d   disconnect\n", connection);
		remove_socket(connection);
	}return false;


	/*
		Disconnect all clients, close send_game_info thread if it run.
		Close main socket and go to section clean up.

		(When host close his game or go out from lobby)
	*/
	case P_Server_exit:
	{
		sendCloseForAll(connection);

		server_running = false;
		game = false;
		closesocket(sListener);

		printf("Server shudown\n");
	}return false;

	/*
		Recv map from host and send to the clients.
	*/
	case P_Map:
	{
		recv(connection, (char*)&map_size, sizeof(int), NULL);

		free(cur_map);
		cur_map = (char*)malloc(map_size);

		// recv map from host
		recv_s(connection, cur_map, map_size);

		// send map to clients
		sendMap(connection, cur_map, map_size);
	}break;

	/*
		Run game on clients machines
	*/
	case P_Start:
	{
		game = true;
		sendStart();
		hGame_thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)send_game_data, NULL, NULL, NULL);
	}break;

	/*
		Recv player info and save it on server.
		Thean in game send info, send it to all
		players.
	*/
	case P_Player:
	{
		recv_s(connection, (char*)&players[connection], sizeof(Tank), NULL);
	}break;

	/*
		Recv destoyed obj info and store on server.
		Also in game send info, send it to all players.
	*/
	case P_Destroy:
	{
		int id;
		recv(connection, (char*)&id, sizeof(int), NULL);
		destroy[nDestroy++] = id;
	}break;

	default:
		printf("unknow packet\n");
		
		sendClose(connection);
		return false;
	}
	return true;
}

/*
	Start in separe thread. And recv Clients messages
*/
void ClientHandler(SOCKET connection)
{
	Packet packettype;
	while (server_running)
	{
		recv(connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProccesPacket(connection, packettype))
			break;
	}
}




// =========================================== Main =======================================================


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

		threads[nConnections] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)connection, NULL, NULL);
		nConnections++;
	}

	printf("server done\n");

	// clean up
	for (int i = 0; i < nConnections; i++)
	{
		shutdown(Connections[i], SD_BOTH);
		closesocket(Connections[i]);
		CloseHandle(threads[i]);
	}

	WSACleanup();

	printf("server clean up\n");
	return 0;
}