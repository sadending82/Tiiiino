#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "Server.h"
#include "Timer.h"
#include "GameDataManager.h"

Server* pServer = new Server;

int main(int argc, char* argv[])
{
	wcout.imbue(locale("korean"));

	pServer->Init();

	delete pServer;
}