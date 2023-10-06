#pragma once
#include "server.h"
#include "../ServerProtocol.h"

void Server::Disconnect(int cID)
{
	mClients[cID].mStateLock.lock();
	if (mClients[cID].mState == eSessionState::ST_FREE) {
		mClients[cID].mStateLock.unlock();
		return;
	}

	cout << "DISCONNECT" << mClients[cID].mUID << endl;

	mMatchListHighTier.remove(cID);
	mMatchListLowTier.remove(cID);

	closesocket(mClients[cID].mSocket);
	mClients[cID].mState = eSessionState::ST_FREE;
	mClients[cID].mStateLock.unlock();
}

int Server::GetNewClientID()
{
	for (int i = MAXGAMESERVER; i < MAX_USER; ++i) {
		mClients[i].mStateLock.lock();
		if (mClients[i].mState == eSessionState::ST_FREE) {
			mClients[i].mState = eSessionState::ST_ACCEPTED;
			mClients[i].mStateLock.unlock();
			return i;
		}
		mClients[i].mStateLock.unlock();
	}
	return INVALID_KEY;
}

int Server::GetNewServerID()
{
	for (int i = 0; i < MAXGAMESERVER; ++i) {
		mServers[i].mStateLock.lock();
		if (mServers[i].mState == eSessionState::ST_FREE) {
			mServers[i].mState = eSessionState::ST_ACCEPTED;
			mServers[i].mStateLock.unlock();
			return i;
		}
		mServers[i].mStateLock.unlock();
	}
	return INVALID_KEY;
}

int Server::GetNewRoomID()
{
	for (int i = 0; i < MAXGAMESERVER*10; ++i) {
		mRooms[i].mStateLock.lock();
		if (mRooms[i].mState == eRoomState::RS_FREE) {
			mRooms[i].mState = eRoomState::RS_READY;
			mRooms[i].mStateLock.unlock();
			return i;
		}
		mServers[i].mStateLock.unlock();
	}
	return INVALID_KEY;
}

void Server::ProcessPacket(int cID, unsigned char* cpacket)
{
	switch (cpacket[1])
	{
	case CL_LOGIN:
	{
		CL_LOGIN_PACKET* p = reinterpret_cast<CL_LOGIN_PACKET*>(cpacket);
		LD_LOGIN_PACKET pac;
		pac.type = LD_LOGIN;
		pac.size = sizeof(LD_LOGIN_PACKET);
		pac.userKey = cID;
		memcpy(pac.id, p->id, sizeof(pac.id));
		memcpy(pac.password, p->password, sizeof(pac.password));
		// send to db server
		mServers[0].DoSend(&pac);
		break;
	}
	case CL_SIGNUP:
	{
		CL_SIGNUP_PACKET* rp = reinterpret_cast<CL_SIGNUP_PACKET*>(cpacket);
		LD_SIGNUP_PACKET sp;
		sp.size = sizeof(sp);
		sp.type = LD_SIGNUP;
		memcpy(sp.id, rp->id, sizeof(rp->id));
		memcpy(sp.password, rp->password, sizeof(rp->password));
		// send to db server
		mServers[0].DoSend(&sp);
		break;
	}
	case CL_MATCH:
	{
		if (mClients[cID].mTier > 3.5)
		{
			mMatchListHighTier.push_back(cID);
			if (mMatchListHighTier.size() == MAX_ROOM_USER / 2)
			{
				mClients[mMatchListHighTier.front()].mMatchStartTime = system_clock::now();
			}
		}
		else
		{
			mMatchListLowTier.push_back(cID);
			if (mMatchListLowTier.size() == MAX_ROOM_USER / 2)
			{
				mClients[mMatchListLowTier.front()].mMatchStartTime = system_clock::now();
			}
		}
		mClients[cID].mState = eSessionState::ST_MATCH;
		break;
	}
	case CL_MATCH_OUT:
	{
		mMatchListHighTier.remove(cID);
		mMatchListLowTier.remove(cID);
		if (mMatchListHighTier.size() > MAX_ROOM_USER / 2)
		{
			mClients[mMatchListHighTier.front()].mMatchStartTime = system_clock::now();
		}
		if (mMatchListLowTier.size() > MAX_ROOM_USER / 2)
		{
			mClients[mMatchListLowTier.front()].mMatchStartTime = system_clock::now();
		}
		mClients[cID].mState = eSessionState::ST_LOBBY;
		break;
	}
	default:
	{
		break;
	}
	}
}

void Server::ProcessPacketServer(int sID, unsigned char* spacket)
{
	switch (spacket[1])
	{
	case GL_LOGIN:
	{
		GL_LOGIN_PACKET* p = reinterpret_cast<GL_LOGIN_PACKET*>(spacket);

		cout << "game server login" << endl;

		break;
	}
	case GL_ROOM_READY:
	{
		GL_ROOM_READY_PACKET* p = reinterpret_cast<GL_ROOM_READY_PACKET*>(spacket);
		LC_MATCH_RESPONSE_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = LC_MATCH_RESPONSE;
		memcpy(packet.gameServerIP, SERVERIP, sizeof(packet.gameServerIP));
		packet.gameServerPortNum = GAMESERVERPORT;

		int uidCount = 0;
		mRooms[p->roomID].mStateLock.lock();
		mRooms[p->roomID].mState = eRoomState::RS_INGAME;
		mRooms[p->roomID].mStateLock.unlock();

		for (auto& player : mReadytoGame)
		{
			if (mClients[player].mRoomID == p->roomID)
			{
				strcpy_s(packet.hashs, mClients[player].mHashs);
				mClients[player].DoSend(&packet);
				mClients[player].mState = eSessionState::ST_INGAME;

				mRooms[mClients[player].mRoomID].UID[uidCount] = mClients[player].mUID;
				uidCount++;
				mReadytoGame.remove(player);
			}
		}

		break;
	}
	case DL_LOGIN_OK:
	{
		cout << "player login ok" << endl;

		DL_LOGIN_OK_PACKET* p = reinterpret_cast<DL_LOGIN_OK_PACKET*>(spacket);

		if (p->connState == TRUE) {
			CheckDuplicateLogin(p->uid);
		}

		mClients[p->userKey].mStateLock.lock();
		mClients[p->userKey].mCredit = p->grade;
		strcpy_s(mClients[p->userKey].mNickName, sizeof(p->nickname), p->nickname);
		strcpy_s(mClients[p->userKey].mID, sizeof(p->id), p->id);
		mClients[p->userKey].mPoint = p->point;
		mClients[p->userKey].mUID = p->uid;
		mClients[p->userKey].mTier = p->tier;
		mClients[p->userKey].mState = eSessionState::ST_LOBBY;
		mClients[p->userKey].mStateLock.unlock();

		LC_LOGIN_OK_PACKET pac;
		pac.type = LC_LOGIN_OK;
		pac.size = sizeof(LC_LOGIN_OK_PACKET);
		pac.id = p->userKey;
		pac.RoomID = 0;
		pac.UID = mClients[p->userKey].mUID;

		mClients[p->userKey].DoSend(&pac);

		break;
	}
	case DL_LOGIN_FAIL:
	{
		cout << "player login fail" << endl;

		DL_LOGIN_FAIL_PACKET* p = reinterpret_cast<DL_LOGIN_FAIL_PACKET*>(spacket);

		LC_LOGIN_FAIL_PACKET pac;
		pac.size = sizeof(LC_LOGIN_FAIL_PACKET);
		pac.type = LC_LOGIN_FAIL;

		mClients[p->userKey].DoSend(&pac);

		break;
	}
	default:
	{
		break;
	}
	}
}

void Server::DoWorker()
{
	while (true)
	{
		DWORD numBytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(mHCP, &numBytes, &key, &over, INFINITE);
		OverEXP* exOver = reinterpret_cast<OverEXP*>(over);
		int client_id = static_cast<int>(key);
		if (FALSE == ret)
		{
			if (exOver->mCompType == eCompType::OP_ACCEPT)
			{
				cout << "Accept Error";
			}
			else
			{
				cout << "GQCS Error on client[" << key << "]\n";
				Disconnect(static_cast<int>(key));
				if (exOver->mCompType == eCompType::OP_SEND)
				{
					delete exOver;
				}
				continue;
			}
		}

		switch (exOver->mCompType)
		{
		case eCompType::OP_ACCEPT:
		{
			SOCKET cSocket = reinterpret_cast<SOCKET>(exOver->mWsaBuf.buf);
			unsigned long long k{};
			memcpy(&k, exOver->mMessageBuf, sizeof(unsigned long long));
			if (INCODE_SERVER_PACKET == k)
			{
				int server_id = GetNewServerID();
				if (server_id != INVALID_KEY)
				{
					mServers[server_id].mSocketID = server_id;
					mServers[server_id].mRecvOver.mCompType = eCompType::OP_SERVER_RECV;
					mServers[server_id].mPrevRemain = 0;
					mServers[server_id].mSocket = cSocket;

					CreateIoCompletionPort(reinterpret_cast<HANDLE>(cSocket), mHCP, server_id, 0);
					mServers[server_id].DoRecv();
					cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
					cout << "server connect\n";
				}
				else
				{
					cout << "Max server exceeded.\n";
				}

				ZeroMemory(&exOver->mOver, sizeof(exOver->mOver));
				exOver->mWsaBuf.buf = reinterpret_cast<CHAR*>(cSocket);
				int addr_size = sizeof(SOCKADDR_IN);
				// if gameserver num more than 1
				/*
				if(mGameServerCnt < mMaxGameServerCnt)
					AcceptEx(mListenSocket, cSocket, exOver->mMessageBuf, BUF_SIZE - 8, addr_size + 16, addr_size + 16, 0, &exOver->mOver);
				else
				*/
				AcceptEx(mListenSocket, cSocket, exOver->mMessageBuf, 0, addr_size + 16, addr_size + 16, 0, &exOver->mOver);
			}
			else {
				int client_id = GetNewClientID();
				if (client_id != INVALID_KEY)
				{
					mClients[client_id].mSocketID = client_id;
					mClients[client_id].mPrevRemain = 0;
					mClients[client_id].mSocket = cSocket;
					CreateIoCompletionPort(reinterpret_cast<HANDLE>(cSocket), mHCP, client_id, 0);
					mClients[client_id].DoRecv();
					cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
					cout << "player connect\n";
				}
				else
				{
					cout << "Max user exceeded.\n";
				}

				ZeroMemory(&exOver->mOver, sizeof(exOver->mOver));
				exOver->mWsaBuf.buf = reinterpret_cast<CHAR*>(cSocket);
				int addr_size = sizeof(SOCKADDR_IN);
				AcceptEx(mListenSocket, cSocket, exOver->mMessageBuf, 0, addr_size + 16, addr_size + 16, 0, &exOver->mOver);

			}
			break;
		}
		case eCompType::OP_RECV:
		{
			if (0 == numBytes)
			{
				Disconnect(client_id);
			}
			int remainData = numBytes + mClients[key].mPrevRemain;
			unsigned char* p = (unsigned char*)exOver->mMessageBuf;
			while (remainData > 0)
			{
				int packetSize = p[0];
				if (packetSize <= remainData)
				{
					ProcessPacket(static_cast<int>(key), p);
					p = p + packetSize;
					remainData = remainData - packetSize;
				}
				else
				{
					break;
				}
			}
			mClients[key].mPrevRemain = remainData;
			if (remainData > 0)
			{
				memcpy(exOver->mMessageBuf, p, remainData);
			}
			mClients[key].DoRecv();
			break;
		}
		case eCompType::OP_SERVER_RECV:
		{
			ServerOverEXP* serverExOver = reinterpret_cast<ServerOverEXP*>(exOver);
			key = serverExOver->mServerTargetID;

			if (0 == numBytes)
			{
				Disconnect(client_id);
			}
			int remainData = numBytes + mServers[key].mPrevRemain;
			unsigned char* p = (unsigned char*)exOver->mMessageBuf;
			while (remainData > 0)
			{
				int packetSize = p[0];
				if (packetSize <= remainData)
				{
					ProcessPacketServer(static_cast<int>(key), p);
					p = p + packetSize;
					remainData = remainData - packetSize;
				}
				else
				{
					break;
				}
			}
			mServers[key].mPrevRemain = remainData;
			if (remainData > 0)
			{
				memcpy(exOver->mMessageBuf, p, remainData);
			}
			mServers[key].DoRecv();
			break;


			break;
		}
		case eCompType::OP_SEND:
		{
			if (0 == numBytes)
			{
				Disconnect(client_id);
			}
			delete exOver;
			break;
		}
		case eCompType::OP_EVENT:
		{
			ProcessEvent((unsigned char*)exOver->mMessageBuf);
			break;
		}
		default:
		{
			break;
		}
		}
	}
}

void Server::Init()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(LOBBYSERVERPORT);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(mListenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	listen(mListenSocket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

	mHCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mListenSocket), mHCP, 9999, 0);

	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OverEXP a_over;
	a_over.mCompType = eCompType::OP_ACCEPT;
	a_over.mWsaBuf.buf = reinterpret_cast<CHAR*>(c_socket);
	AcceptEx(mListenSocket, c_socket, a_over.mMessageBuf, BUF_SIZE - 8, addr_size + 16, addr_size + 16, 0, &a_over.mOver);

	SOCKET LDsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(DBSERVERPORT);
	inet_pton(AF_INET, SERVERIP, &serverAddr.sin_addr);

	if (connect(LDsocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		cout << "DBconnection failed" << endl;
	}
	else {
		cout << "DBconnection success" << endl;
		OverEXP ss_over;
		ss_over.mCompType = eCompType::OP_ACCEPT;
		ss_over.mWsaBuf.buf = reinterpret_cast<CHAR*>(LDsocket);

		// DB server setting

		int server_id = GetNewServerID();
		if (server_id != INVALID_KEY)
		{
			mServers[server_id].mSocketID = server_id;
			mServers[server_id].mRecvOver.mCompType = eCompType::OP_SERVER_RECV;
			mServers[server_id].mPrevRemain = 0;
			mServers[server_id].mSocket = LDsocket;

			CreateIoCompletionPort(reinterpret_cast<HANDLE>(LDsocket), mHCP, server_id, 0);
			mServers[server_id].DoRecv();
			LDsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			cout << "server connect\n";
		}
		else
		{
			cout << "Max server exceeded.\n";
		}
	}

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
		mWorkerThreads.emplace_back(thread(&Server::DoWorker, this));

	pTimer = new Timer;
	pTimer->Init(mHCP);

	for (auto& th : mWorkerThreads)
		th.join();

	delete pTimer;
	closesocket(mListenSocket);
	WSACleanup();
}

void Server::ProcessEvent(unsigned char* cmessage)
{
	switch (cmessage[1]) {
	case eEVENT_TYPE::EV_MATCH_UP:
	{
		// match room max
		if (mMatchListHighTier.size() >= MAX_ROOM_USER)
		{
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int player_id = mMatchListHighTier.front();

				LG_USER_INTO_GAME_PACKET packet;
				packet.size = sizeof(packet);
				packet.type = LG_USER_INTO_GAME;
				packet.roomID = GetNewRoomID();// need update		
				_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10);
				strcpy_s(packet.hashs, mClients[player_id].mHashs);
				strcpy_s(packet.name, sizeof(mClients[player_id].mNickName), mClients[player_id].mNickName);
				strcpy_s(packet.id, sizeof(mClients[player_id].mID), mClients[player_id].mID);
				packet.uID = mClients[player_id].mUID;
				packet.roomMax = MAX_ROOM_USER;
				mClients[player_id].mRoomID = packet.roomID;

				mServers[1].DoSend(&packet);

				mMatchListHighTier.pop_front();
				mReadytoGame.push_back(player_id);
			}
		}
		if (mMatchListLowTier.size() >= MAX_ROOM_USER)
		{
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int player_id = mMatchListLowTier.front();

				LG_USER_INTO_GAME_PACKET packet;
				packet.size = sizeof(packet);
				packet.type = LG_USER_INTO_GAME;
				packet.roomID = GetNewRoomID();// need update
				_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10);
				strcpy_s(packet.hashs, mClients[player_id].mHashs);
				strcpy_s(packet.name, sizeof(mClients[player_id].mNickName), mClients[player_id].mNickName);
				strcpy_s(packet.id, sizeof(mClients[player_id].mID), mClients[player_id].mID);
				packet.uID = mClients[player_id].mUID;
				packet.roomMax = MAX_ROOM_USER;
				mClients[player_id].mRoomID = packet.roomID;

				mServers[1].DoSend(&packet);
				
				mMatchListLowTier.pop_front();
				mReadytoGame.push_back(player_id);
			}
		}

		// half room max
		system_clock::time_point tTime = system_clock::now();
		if (mMatchListHighTier.size() >= MAX_ROOM_USER / 2) // high list
		{
			if (tTime - mClients[mMatchListHighTier.front()].mMatchStartTime >= milliseconds(20000))
				{
					for (int i = 0; i < mMatchListHighTier.size(); ++i)
					{
						int player_id = mMatchListHighTier.front();

						LG_USER_INTO_GAME_PACKET packet;
						packet.size = sizeof(packet);
						packet.type = LG_USER_INTO_GAME;
						packet.roomID = GetNewRoomID();//need update
						_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10);
						strcpy_s(packet.hashs, mClients[player_id].mHashs);
						strcpy_s(packet.name, sizeof(mClients[player_id].mNickName), mClients[player_id].mNickName);
						strcpy_s(packet.id, sizeof(mClients[player_id].mID), mClients[player_id].mID);
						packet.uID = mClients[player_id].mUID;
						packet.roomMax = mMatchListHighTier.size();
						mClients[player_id].mRoomID = packet.roomID;

						mServers[1].DoSend(&packet);

						mMatchListHighTier.pop_front();
						mReadytoGame.push_back(player_id);
					}
				}
			else
				{
					// count down packet?
				}
		}
		if (mMatchListLowTier.size() >= MAX_ROOM_USER / 2)
		{
			if (tTime - mClients[mMatchListLowTier.front()].mMatchStartTime >= milliseconds(20000))
			{
				for (int i = 0; i < mMatchListLowTier.size(); ++i)
				{
					int player_id = mMatchListLowTier.front();

					LG_USER_INTO_GAME_PACKET packet;
					packet.size = sizeof(packet);
					packet.type = LG_USER_INTO_GAME;
					packet.roomID = GetNewRoomID();//need update
					strcpy_s(packet.name, sizeof(mClients[player_id].mNickName), mClients[player_id].mNickName);
					strcpy_s(packet.id, sizeof(mClients[player_id].mID), mClients[player_id].mID);
					packet.uID = mClients[player_id].mUID;
					packet.roomMax = mMatchListLowTier.size();
					_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10);
					strcpy_s(packet.hashs, mClients[player_id].mHashs);
					packet.roomMax = mMatchListLowTier.size();
					mClients[player_id].mRoomID = packet.roomID;

					mServers[1].DoSend(&packet);

					mMatchListLowTier.pop_front();
					mReadytoGame.push_back(player_id);
				}
			}
			else
			{
				// count down packet?
			}
		}

		EV_UpdateMatchPacket p;
		p.size = sizeof(EV_UpdateMatchPacket);
		p.type = eCompType::OP_EVENT;
		pTimer->PushEvent(1, eEVENT_TYPE::EV_MATCH_UP, 5000, reinterpret_cast<unsigned char*>(&p));

		break;
	}
	case eEVENT_TYPE::EV_COUNT_DOWN:
	{
		break;
	}
	default:
	{
		break;
	}
	}
}

void Server::CheckDuplicateLogin(int uid)
{
	int target = -1;
	for (int i = MAXGAMESERVER; i < MAX_USER; ++i) {
		mClients[i].mStateLock.lock();
		if (mClients[i].mUID == uid) {
			target = i;
			mClients[i].mStateLock.unlock();
			break;
		}
		mClients[i].mStateLock.unlock();
	}
	if (target != -1) {
		if (mClients[target].mState == eSessionState::ST_INGAME) {
			SendDiconnectPacketToGameServer(target, uid, mClients[target].mRoomID);
		}
		else {
			Disconnect(target);
		}
	}
}

void Server::SendDiconnectPacketToGameServer(int key, int uid, int roomID)
{
	LG_USER_DISCONNECT_PACKET p;
	p.size = sizeof(p);
	p.type = LG_USER_DISCONNECT;
	p.uID = uid;
	p.roomID = roomID;
	
	mClients[key].DoSend(reinterpret_cast<char*>(&p));
}
