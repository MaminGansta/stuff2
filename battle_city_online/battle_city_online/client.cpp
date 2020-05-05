#pragma warning (disable: 4996)

#define WM_SETMAP (WM_USER)
#define WM_START (WM_USER + 1)
#define WM_UPDATE_LOG (WM_USER + 2)


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


// from Main_window
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

int nPlayers = 0;
Tank players[4];
float init_pos[4];

int nDestroy = 0;
int destroy[10];

bool client_runnig = true;

struct Client
{
	HWND main_window;
	SOCKET Connection = 0;
	HANDLE server_callback = 0;
	std::wstring log;

	// game data
	std::vector<std::pair<int, vec2>> loaded_map;


	Client()
	{
		WSADATA	wsd;

		// load lib
		if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
		{
			log += L"Failed to load Winsock library!\r\n";
		}
	}

	void Connect(const std::wstring& ip, int port)
	{
		bool ProccesPacket(Packet packettype, Client & client);
		void ClientHandler(Client * client);

		if (Connection != 0)
			Disconnect();

		// conver wchar_t to char
		char ip_utf8[32];
		stbi_convert_wchar_to_utf8(ip_utf8, 32, ip.c_str());

		SOCKADDR_IN client;
		hostent* host = NULL;

		client.sin_family = AF_INET;
		client.sin_port = htons(port);
		client.sin_addr.s_addr = inet_addr(ip_utf8);


		Connection = socket(AF_INET, SOCK_STREAM, NULL);
		if (connect(Connection, (SOCKADDR*)&client, sizeof(client)))
		{
			log += L"connection failled\r\n";
			return;
		}
		log += L"Connection seccess\r\n";

		client_runnig = true;
		// create thread with receive msg hadnler
		server_callback = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)this, NULL, NULL);

		SendMessage(main_window, WM_UPDATE_LOG, 0, 0);
	}

	void Disconnect()
	{
		if (Connection)
		{
			Packet packet_exit = P_Exit;
			send(Connection, (char*)&packet_exit, sizeof(Packet), NULL);
			shutdown(Connection, SD_BOTH);

			SendMessage(main_window, WM_UPDATE_LOG, 0, 0);
		}

		Connection = 0;
		client_runnig = false;
	}

	~Client()
	{
		if (Connection)
			Disconnect();

		if (server_callback)
			CloseHandle(server_callback);
		
		if (Connection)
			closesocket(Connection);

		WSACleanup();
	}

	void send_map(const std::vector<std::pair<int, vec2>>& map)
	{
		if (Connection == 0) return;

		Packet packettype = P_Map;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);
		
		auto [data, size] = map_to_data(map);
		size *= sizeof(wchar_t);

		send(Connection, (char*)&size, sizeof(int), NULL);
		send(Connection, (char*)data, size, NULL);

		SendMessage(main_window, WM_UPDATE_LOG, 0, 0);
	}

	void start_game()
	{
		Packet packettype_send = P_Start;
		send(Connection, (char*)&packettype_send, sizeof(Packet), NULL);
	}

	void send_server_close()
	{
		Packet packettype_send = P_Server_exit;
		send(Connection, (char*)&packettype_send, sizeof(Packet), NULL);
	}

	void send_game_data()
	{
		// send input text to the server
		char msg[128] = "message\r\n";
		Packet packettype_send = P_ChatMessage;

		//gets_s(msg);
		if (!Connection) return;


		send(Connection, (char*)&packettype_send, sizeof(Packet), NULL);
		send(Connection, msg, sizeof(msg), NULL);

		Sleep(5000);
	}

};



// ============================== Server callback =====================================

bool ProccesPacket(Packet packettype, Client& client)
{
	if (packettype != 10)
	{
		wchar_t buf[32];
		output("packet %d\n", packettype);

		swprintf_s(buf, L"packet %d \r\n", packettype);
		client.log += buf;
	}
	SendMessage(client.main_window, WM_UPDATE_LOG, 0, 0);

	switch (packettype)
	{

	case P_InitConnection:
	{
		// somethig
	}break;

	case P_ChatMessage:
	{
		//char msg[128];
		//recv(client.Connection, msg, sizeof(msg), NULL);
		//doutput("%s\n", msg);
	}break;

	case P_Exit:
	{
		client.log += L"Server disconect this client \r\n";
		runnig = false;
		client_runnig = false;
		
		client.Connection = 0;
	}return false;

	case P_Map:
	{
		int size = 0;
		recv(client.Connection, (char*)&size, sizeof(int), NULL);

		char* map = (char*)malloc(size);
		recv_s(client.Connection, map, size);

		client.loaded_map = parse_map((wchar_t*)map);

		free(map);
		// Send message to the widnow to update the map
		client.log += L"map was loaded\r\n";
		output("map was loaded\n");
		SendMessage(client.main_window, WM_SETMAP, 0, 0);
	}break;

	case P_Start:
	{
		SendMessage(client.main_window, WM_START, 0, 0);
	}break;

	case P_Player:
	{
		int size = 0;
		recv(client.Connection, (char*)&size, sizeof(int), NULL);

		nPlayers = size;
		for (int i = 0; i < size; i++)
			recv(client.Connection, (char*)&players[i], sizeof(Tank), NULL);

	}break;

	case P_Destroy:
	{
		recv(client.Connection, (char*)&nDestroy, sizeof(int), NULL);
		
		int id = 0;
		for (int i = 0; i < nDestroy; i++)
		{
			recv(client.Connection, (char*)&id, sizeof(int), NULL);
			destroy[i] = id;
		}
	}break;

	case P_InitPosition:
	{
		recv(client.Connection, (char*)init_pos, sizeof(float) * 2, NULL);
	}break;

	default:
		client.log += L"unkown packet\r\n";
		return false;
	}

	SendMessage(client.main_window, WM_UPDATE_LOG, 0, 0);
	return true;
}



void ClientHandler(Client* client)
{
	Packet packettype;
	while (client_runnig)
	{
		recv(client->Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProccesPacket(packettype, *client))
			break;
	}
}

