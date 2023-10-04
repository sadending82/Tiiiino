#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "DB.h"

DB* g_pDB = new DB;

#define DBTest

int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));

<<<<<<< Updated upstream
#ifdef DBTest
	g_pDB->ConnectDB();

	g_pDB->DisconnectDB();
#endif
=======
	DB* pDB = new DB;
	Socket* pSocket = new Socket;
	Security* pSecurity = new Security;

	pDB->SetmSecurity(pSecurity);
	pDB->ConnectDB();

	pSocket->ServerReady(pDB);

	pDB->DisconnectDB();
>>>>>>> Stashed changes
}