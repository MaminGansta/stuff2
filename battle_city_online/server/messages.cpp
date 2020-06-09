

// remove connection from array
void remove_socket(SOCKET sock)
{
	int i = 0;
	for (; i < nConnections; i++)
		if (Connections[i] == sock) break;

	std::unique_lock<std::shared_mutex> lock(mutex);
	std::swap(Connections[i], Connections[nConnections - 1]);
	std::swap(threads[i], threads[nConnections - 1]);
	nConnections--;
}




// Connection messages

void sendTest(int index)
{
	Packet packettype = P_Test;
	send(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
}


void sendInit(int index)
{
	Packet packettype = P_InitConnection;
	send(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
}


void sendClose(SOCKET sock)
{
	printf("Client has been disconected, socket: %d\n", sock);

	Packet packettype = P_Exit;
	send(sock, (char*)&packettype, sizeof(Packet), NULL);
	shutdown(sock, CF_BOTH);

	remove_socket(sock);
}



void sendCloseForAll(SOCKET sock)
{
	for (int i = 0; i < nConnections; i++)
	{
		if (Connections[i] != sock)
		{
			Packet packettype = P_Exit;
			send(Connections[i], (char*)&packettype, sizeof(Packet), NULL);
		}
	}
}



void sendServerFull(SOCKET connection)
{
	Packet packettype = P_ServerFull;
	send(connection, (char*)&packettype, sizeof(Packet), NULL);
}



// Chat Message
void sendMessage(int index, const std::wstring& msg)
{
	Packet packettype = P_ChatMessage;
	send(Connections[index], (char*)&packettype, sizeof(Packet), NULL);

	int len = msg.size();
	send(Connections[index], (char*)&len, sizeof(int), NULL);
	send(Connections[index], (char*)msg.c_str(), sizeof(wchar_t) * len, NULL);
}



// Game map message
void sendMap(SOCKET sock, char* map, int size)
{
	for (int i = 0; i < nConnections; i++)
	{
		Packet packettype = P_Map;
		if (Connections[i] != sock)
		{
			printf("send map to %d\n", Connections[i]);

			send(Connections[i], (char*)&packettype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&size, sizeof(int), NULL);
			send(Connections[i], map, size, NULL);
		}
	}
}


void sendStart()
{
	srand(time(0));
	int init_pos_ind = (rand() % 8) & 0x11110;
	Packet packet;

	// send init position
	for (int i = 0; i < nConnections; i++)
	{
		printf("send init pos to %d\n", Connections[i]);
		
		packet = P_InitPosition;
		send(Connections[i], (char*)&packet, sizeof(Packet), NULL);
		send(Connections[i], (char*)(init_pos + (2*i + init_pos_ind) % 8), sizeof(float) * 2, NULL);
	}

	// send game start message
	for (int i = 0; i < nConnections; i++)
	{
		printf("send start to %d\n", Connections[i]);

		packet = P_Start;
		send(Connections[i], (char*)&packet, sizeof(int), NULL);
	}
}
