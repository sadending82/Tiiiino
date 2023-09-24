#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "cDB.h"
#include "cSocket.h"

DB* g_pDB = new DB;
Socket* g_pSocket = new Socket;
//#define DBTest

int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));
	g_pDB->ConnectDB();
	g_pSocket->ServerReady(g_pDB);

	g_pDB->DisconnectDB();
}