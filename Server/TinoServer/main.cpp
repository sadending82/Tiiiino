#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "Server.h"
#include "Timer.h"

Server* pServer = new Server;
Timer* pTimer = new Timer;

int main(int argc, char* argv[])
{
	wcout.imbue(locale("korean"));

	pServer->Init();
	pTimer->Init(pServer->GetHandle());

	delete pTimer;
	delete pServer;
}