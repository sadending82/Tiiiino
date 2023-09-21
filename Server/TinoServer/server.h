#pragma once
#include "Session.h"

constexpr int MAX_THREAD = 6;
constexpr int INVALID_KEY = -1;

class Server {
public:
	void Disconnect(int cID);
	int GetNewClientID();
	void ProcessPacket(int cID, char* cpacket);
	void DoWorker();
	void Init();
	void ProcessEvent(char* cmessage);
	HANDLE GetHandle();
public:
	array <Session, MAX_USER> mClients;
private:
	SOCKET mListenSocket;
	HANDLE mHCP;
	vector <thread> mWorkerThreads;
};