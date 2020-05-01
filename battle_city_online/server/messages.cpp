


// Connection

void sendInit(int index)
{
	Packet packettype = P_InitConnection;
	send(Connections[nConnections], (char*)&packettype, sizeof(Packet), NULL);
}

void sendClose(int index)
{
	Packet packettype = P_Exit;
	send(Connections[nConnections], (char*)&packettype, sizeof(Packet), NULL);
}

void sendServerFull(int index)
{
	Packet packettype = P_ServerFull;
	send(Connections[nConnections], (char*)&packettype, sizeof(Packet), NULL);
}


// Chat Message

void sendMessage(int index, const std::wstring& msg)
{
	Packet packettype = P_ChatMessage;
	send(Connections[nConnections], (char*)&packettype, sizeof(Packet), NULL);

	int len = msg.size();
	send(Connections[nConnections], (char*)&len, sizeof(int), NULL);
	send(Connections[nConnections], (char*)msg.c_str(), sizeof(wchar_t) * len, NULL);
}


// Game data