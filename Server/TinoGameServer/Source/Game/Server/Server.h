#pragma once
#include "../Network/Network.h"

class Server : public Network
{
public:
	Server();
	virtual ~Server();

private:


private:
	SOCKADDR_IN mServerAddr;
};

