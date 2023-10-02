#include <iostream>
#include "LobbyServer.h"
#include "../MainServer/MainServer.h"
#include "../../../../../ServerProtocol.h"
#include "../../Utility/Utility.h"

LobbyServer::~LobbyServer()
{
}

void LobbyServer::init()
{
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	mSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (bind(mSocket, reinterpret_cast<sockaddr*>(&server_addr),
		sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cout << "bind error\n";
		return;
	}
	OVERLAPPED overex;
	ZeroMemory(&overex, sizeof(overex));
	server_addr.sin_port = htons(LOBBYSERVERPORT);
	inet_pton(AF_INET, LOOBYSERVER_ADDR, &server_addr.sin_addr);

	LPFN_CONNECTEX Conn;
	GUID guid = WSAID_CONNECTEX;
	DWORD dwbyte{};

	WSAIoctl(mSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid),
		&Conn, sizeof(Conn),
		&dwbyte, NULL, NULL);

	WSABUF temp;
	unsigned char tbuf[120];
	memcpy(tbuf, &INCODE_SERVER_PACKET, sizeof(unsigned long long));
	temp.buf = (char*)tbuf;
	temp.len = sizeof(tbuf);

	if (Conn(mSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr),
		tbuf, sizeof(tbuf),
		NULL, reinterpret_cast<LPOVERLAPPED>(&overex)) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			std::cout << "error\n";
			return;
			//error ! 
		}
	}
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mSocket), gMainServer->GetIOCPHandle(), 0, 0);
	AcceptSetting(eSocketState::ST_ACCEPT, eCOMMAND_IOCP::CMD_SERVER_RECV, mSocket);
	PreRecvPacket(NULL, 0);
	RecvPacket();

	std::cout << "Lobby Connect Complete\n";
}
