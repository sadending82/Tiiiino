#include "Server/MainServer/MainServer.h"
#include "Server/LobbyServer/LobbyServer.h"
#include "Object/Player/Player.h"

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")



int main()
{
	std::wcout.imbue(std::locale("korean"));
	//LobbyServer l;
	gMainServer = new MainServer();
	gMainServer->init();
	//l.init();
	gMainServer->run();

	delete gMainServer;

	return 0;
}