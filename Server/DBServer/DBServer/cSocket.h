#pragma once
#include "stdafx.h"
#include "cSession.h"
#include "cDB.h"

constexpr int DBSERVERPORT = 3700;

class Socket {
public:

	int SetKey();
	void Disconnect(int key);
	void WorkerFunc();
	void ServerReady(DB* pDB);
	void processPacket(int key, unsigned char* buf);

	void Setm_pDB(DB* pDB) { m_pDB = pDB; }

	bool CheckLogin(int key, unsigned char* buf);

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;

	DB* m_pDB = NULL;

	array<Session, MAXLOBBY + 1> mSessions;


};