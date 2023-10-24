#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "cDB.h"
#include "cSocket.h"

int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));

	Socket* pSocket = new Socket;

#ifdef RUN_DB
	Security* pSecurity = new Security;
	DB* pDB = new DB;
	pDB->SetmSecurity(pSecurity);
	pDB->ConnectDB();
	pSocket->Setm_pDB(pDB);
#endif

	pSocket->ServerReady();

#ifdef RUN_DB
	pDB->DisconnectDB();
#endif
}