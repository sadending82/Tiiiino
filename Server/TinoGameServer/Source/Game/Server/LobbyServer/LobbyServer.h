#pragma once
#include "../Server.h"
class LobbyServer : public Server
{
public:
	void init();


protected:
	//SOCKADDR_IN server_addr;
	//WSA_OVER_EX recv_expover;
	//int		_prev_size;
	//bool bIsConnected;	//임시 변수
	const char* SERVER_ADDR = "127.0.0.1";
};

