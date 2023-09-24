#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "cDB.h"

DB* g_pDB = new DB;

//#define DBTest

int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));

#ifdef DBTest
	g_pDB->ConnectDB();
	g_pDB->DisconnectDB();
#endif
}