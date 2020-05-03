


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
	Packet packettype = P_Exit;
	send(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
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


// Game data
void sendMap(int index, char* map, int size)
{
	printf("send map to %d\n", index);

	for (int i = 0; i < nConnections; i++)
	{
		Packet packettype = P_Map;
		if (i != index)
		{
			printf("%d\n", i);

			send(Connections[i], (char*)&packettype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&size, sizeof(int), NULL);
			send(Connections[i], map, size, NULL);
		}
	}
}

void sendStart(int index)
{
	for (int i = 0; i < nConnections; i++)
	{
		if (i != index)
		{
			Packet packettype = P_Start;
			send(Connections[i], (char*)&packettype, sizeof(Packet), NULL);
		}
	}
}
