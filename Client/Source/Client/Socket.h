#pragma once

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "Runtime/Core/Public/Math/Vector.h"

#define SERVERIP 127.0.0.1
#define SERVERPORT 3500
#define BUFSIZE 200

class Socket
{
public:
	Socket() {};
	~Socket() {};

	bool Init();
	bool Connect();
	void ReceivePacket();
	void ProcessPacket(char* buf);

private:
	SOCKET	sock;

};