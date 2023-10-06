#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "cDB.h"
#include "cSocket.h"
//#define DBTest


int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));

	DB* pDB = new DB;
	Socket* pSocket = new Socket;
	Security* pSecurity = new Security;

	pDB->SetmSecurity(pSecurity);
	pDB->ConnectDB();

	pDB->UpdateUserDepartment(9, 10);

	pSocket->ServerReady(pDB);

	pDB->DisconnectDB();
}