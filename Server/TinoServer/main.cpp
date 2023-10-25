#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "Server.h"
#include "Timer.h"
#include "GameDataManager.h"

Server* pServer = new Server;
GameDataManager* pGDM = new GameDataManager;

int main(int argc, char* argv[])
{
	wcout.imbue(locale("korean"));

	pGDM->LoadCouponData();

	pServer->Init();

	delete pServer;
}