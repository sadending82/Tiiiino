#pragma once
#include "Session.h"
#include "Timer.h"

constexpr int MAX_THREAD = 6;
constexpr int INVALID_KEY = -1;

class Server {
public:
	void Disconnect(int cID);
	int GetNewClientID();
	void ProcessPacket(int cID, char* cpacket);
	void DoWorker();
	void Init();
<<<<<<< Updated upstream
	void ProcessEvent(char* cmessage);
public:
	array <Session, MAX_USER> mClients;
=======
	void ProcessEvent(unsigned char* cmessage);

	void CheckDuplicateLogin(int uid);

	void SendDiconnectPacketToGameServer(int key, int uid, int roomID);
public:
	array <Session, MAX_USER> mClients;
	array <Session, MAXGAMESERVER> mServers;
	list <int> mMatchListHighTier;
	list <int> mMatchListLowTier;
	list <int> mReadytoGame;
>>>>>>> Stashed changes
private:
	SOCKET mListenSocket;
	HANDLE mHCP;
	vector <thread> mWorkerThreads;
	Timer* pTimer;
};