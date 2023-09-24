#pragma once
#include "stdafx.h"
#include "cSession.h"
#include "cDB.h"

constexpr int DBSERVERPORT = 3700;

class Socket {
public:

	int SetKey();
	void WorkerFunc();
	void ServerReady();
	void processPacket(int key, unsigned char* buf);

	void Setm_pDB(DB* pDB) { m_pDB = pDB; }

private:
	SOCKADDR_IN mServerAddr;
	HANDLE mHcp;
	SOCKET mListenSocket;

	DB* m_pDB = NULL;

	array<Session, MAXLOBBY + 1> mSessions;


};