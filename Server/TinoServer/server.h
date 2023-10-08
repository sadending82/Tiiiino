#pragma once
#include "Session.h"
#include "Timer.h"
#include "room.h"
#include "../ServerProtocol.h"

constexpr int MAX_THREAD = 6;
constexpr int INVALID_KEY = -1;

class Server {
public:
	void Disconnect(int cID);
	int GetNewClientID();
	int GetNewServerID();
	int GetNewRoomID();
	void ProcessPacket(int cID, unsigned char* cpacket);
	void ProcessPacketServer(int sID, unsigned char* spacket);
	void DoWorker();
	void Init();
	void ProcessEvent(unsigned char* cmessage);

	void CheckDuplicateLogin(int uid);

	void SendDiconnectPacketToGameServer(int key, int uid, int roomID);
	void SendMatchResult(int key);
public:
	array <Session, MAX_USER> mClients;
	array <Session, MAXGAMESERVER> mServers;
	array <Room, MAXGAMESERVER * 10> mRooms;
	list <int> mMatchListHighTier;
	list <int> mMatchListLowTier;
	list <int> mReadytoGame;
private:
	SOCKET mListenSocket;
	HANDLE mHCP;
	vector <thread> mWorkerThreads;
	Timer* pTimer;
};