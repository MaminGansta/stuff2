#pragma warning (disable: 4996)

#define WM_SETMAP (WM_USER)
#define WM_START (WM_USER + 1)

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
	P_Start
};

bool client_runnig = true;

struct Client
{
	HWND main_window;
	SOCKET Connection = 0;
	HANDLE server_callback = 0;

	// game data
	std::vector<std::pair<int, vec2>> loaded_map;


	Client()
	{
		WSADATA	wsd;

		// load lib
		if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
		{
			doutput("Failed to load Winsock library!\n");
		}
	}

	void Connect(const std::wstring& ip, int port)
	{
		bool ProccesPacket(Packet packettype, Client & client);
		void ClientHandler(Client * client);

		if (Connection != 0) return;

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
			doutput("connection failled\n");
			return;
		}
		doutput("Connection seccess\n");

		client_runnig = true;

		// create thread with receive msg hadnler
		server_callback = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)this, NULL, NULL);
	}

	void Disconnect()
	{
		Packet packet_exit = P_Exit;
		send(Connection, (char*)&packet_exit, sizeof(Packet), NULL);
		shutdown(Connection, SD_BOTH);
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
		Packet packettype = P_Map;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);
		
		auto [data, size] = map_to_data(map);
		size *= sizeof(wchar_t);

		send(Connection, (char*)&size, sizeof(int), NULL);

		//int sended = 0;
		//while (sended != size)
		send(Connection, (char*)data, size, NULL);
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
		char msg[128] = "message\n";
		Packet packettype_send = P_ChatMessage;

		//gets_s(msg);
		if (!Connection) return;


		send(Connection, (char*)&packettype_send, sizeof(Packet), NULL);
		send(Connection, msg, sizeof(msg), NULL);

		Sleep(5000);
	}
};



bool ProccesPacket(Packet packettype, Client& client)
{
	doutput("%d\n", packettype);

	switch (packettype)
	{

	case P_InitConnection:
	{

	}break;

	case P_ChatMessage:
	{
		char msg[128];
		recv(client.Connection, msg, sizeof(msg), NULL);
		doutput("%s\n", msg);
	}break;

	case P_Exit:
	{
		doutput("Server shutdown\n");
		client.Connection = 0;
	}return false;

	case P_Map:
	{
		doutput(L"msg\n");

		int size = 0;
		recv(client.Connection, (char*)&size, sizeof(int), NULL);

		char* map = new char[size];
		recv(client.Connection, map, size, NULL);
		
		client.loaded_map = parse_map((wchar_t*)map);

		// Send message to the widnow to update the map
		SendMessage(client.main_window, WM_SETMAP, 0, 0);
	}break;

	case P_Start:
	{
		SendMessage(client.main_window, WM_START, 0, 0);
	}break;

	default:
		doutput("unkown packet\n");
		return false;
	}

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

