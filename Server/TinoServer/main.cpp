#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "server.h"

Server* g_pServer = new Server;

int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));

	g_pServer->init();


	delete g_pServer;
}