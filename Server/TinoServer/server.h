#pragma once
#include "Session.h"
#include "Timer.h"
#include "../ServerProtocol.h"

constexpr int MAX_THREAD = 6;
constexpr int INVALID_KEY = -1;

class Server {
public:
	void Disconnect(int cID);
	int GetNewClientID();
	int GetNewServerID();
	void ProcessPacket(int cID, unsigned char* cpacket);
	void ProcessPacketServer(int sID, unsigned char* spacket);
	void DoWorker();
	void Init();
	void ProcessEvent(unsigned char* cmessage);
public:
	array <Session, MAX_USER> mClients;
	array <Session, MAXGAMESERVER> mServers;
private:
	SOCKET mListenSocket;
	HANDLE mHCP;
	vector <thread> mWorkerThreads;
	Timer* pTimer;
};