#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "cDB.h"
#include "cSocket.h"
//#define DBTest

int main(int argc, char* argv[])
{
	//std::wcout.imbue(std::locale("korean"));
	DB* g_pDB = new DB;
	Socket* g_pSocket = new Socket;

	g_pDB->ConnectDB();
	g_pSocket->ServerReady(g_pDB);


	//g_pDB->DisconnectDB();
}