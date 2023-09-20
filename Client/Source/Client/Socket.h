#pragma once

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "Runtime/Core/Public/Math/Vector.h"
#include "../../../protocol.h"

class Socket
{
public:
	Socket() {};
	~Socket() {};

	bool Init();
	bool Connect();
	void ReceivePacket();
	bool SendPacket(char* buf);
	void ProcessPacket(char* buf);

	SOCKET Get_m_sock() { return m_sock; }

private:
	SOCKET	m_sock;

};