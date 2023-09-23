#pragma once
#include "../Network/Network.h"

class Server : public Network
{
public:
	Server();
	~Server();

private:


private:
	SOCKADDR_IN mServerAddr;
};

