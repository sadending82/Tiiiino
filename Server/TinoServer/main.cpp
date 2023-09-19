#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "server.h"
#include "timer.h"

Server* g_pServer = new Server;
Timer* timer = new Timer;

int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));

	g_pServer->init();
	timer->init(g_pServer->get_handle());

	delete timer;
	delete g_pServer;
}