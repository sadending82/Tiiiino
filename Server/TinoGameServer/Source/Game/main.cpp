#include "Server/MainServer/MainServer.h"
#include "Object/Player/Player.h"

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")



int main()
{
	std::wcout.imbue(std::locale("korean"));
	gMainServer = new MainServer();
	gMainServer->init();
	gMainServer->run();

	delete gMainServer;

	return 0;
}