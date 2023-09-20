#pragma once

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "Runtime/Core/Public/Math/Vector.h"

class ATinoPlayerController;

using namespace std;

#define SERVERIP 127.0.0.1
#define SERVERPORT 3500
#define BUFSIZE 4096
#define SERVER_IP "127.0.0.1"



class cCharacter { 
public:
	cCharacter() {};
	~cCharacter() {};

	int SessionId;

	float X;
	float Y;
	float Z;

	float rx;
	float ry;
	float rz;

	float vx;
	float vy;
	float vz;

	int animNum = 0;

};

class CLIENT_API cSocket : public FRunnable // 서버 접속 및 패킷 처리
{
public:
	cSocket();
	~cSocket();

	bool InitializeSocket();
	bool Connect();

	void ReceivePacket();
	void ProcessPacket(char* buf);

	void SetPlayerController(ATinoPlayerController* pPlayerController);

	void CloseSocket();

	FRunnableThread* thread;
	FThreadSafeCounter threadCounter;

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

	bool StartListen();
	void StopListen();

	static cSocket* GetSingleton()
	{
		static cSocket instance;
		return &instance;
	}


private:
	SOCKET	sock; // 
	char rcvBuf[BUFSIZE]; // 수신 버퍼

	ATinoPlayerController* m_playerController;

};