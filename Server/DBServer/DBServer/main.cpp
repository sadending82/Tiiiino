#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "DB.h"

DB* g_pDB = new DB;

int main(int argc, char* argv[])
{
	std::wcout.imbue(std::locale("korean"));

	bool b = g_pDB->ConnectDB();
}