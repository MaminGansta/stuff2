#pragma warning (disable: 4996)

enum Packet
{
	P_Server_exit,

	// connection
	P_InitConnection,
	P_Exit,
	P_ServerFull,
	P_Test,

	// chat 
	P_ChatMessage

	// game
};

bool client_runnig = true;

bool ProccesPacket(Packet packettype, SOCKET& Connection)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		char msg[128];
		recv(Connection, msg, sizeof(msg), NULL);
		doutput("%s\n", msg);
	}break;

	case P_Exit:
	{
		doutput("Server shutdown\n");
		Connection = 0;
	}return false;

	default:
		doutput("unkown packet\n");
		return false;
	}
}


void ClientHandler(SOCKET Connection)
{
	Packet packettype;
	while (client_runnig)
	{
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProccesPacket(packettype, Connection))
			break;
	}
}



struct Client
{
	SOCKET Connection = 0;
	HANDLE server_callback = 0;

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
		server_callback = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)Connection, NULL, NULL);
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