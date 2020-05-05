


// Connection

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


void sendClose(int index)
{
	printf("Client has been disconected socket: %d\n", Connections[index]);

	Packet packettype = P_Exit;
	send(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
	shutdown(Connections[index], CF_BOTH);

	// remove from array
	std::unique_lock<std::shared_mutex> lock(mutex);
	std::swap(Connections[index], Connections[nConnections]);
	std::swap(threads[index], threads[nConnections]);
	nConnections--;
}



void sendCloseForAll()
{
	for (int i = 0; i < nConnections; i++)
	{
		Packet packettype = P_Exit;
		send(Connections[i], (char*)&packettype, sizeof(Packet), NULL);
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



// Game map
void sendMap(int index, char* map, int size)
{
	for (int i = 0; i < nConnections; i++)
	{
		Packet packettype = P_Map;
		if (i != index)
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
	int init_pos_ind = (rand() % 8) ^ 1;
	Packet packet;

	printf("init pos ind  %d \n", init_pos_ind);

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
	Sleep(10);
}
