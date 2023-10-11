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

	DEBUGMSGONEPARAM("DISCONNECT [%s]\n", mClients[cID].mID);

	mMatchListHighTier.remove(cID);
	mMatchListLowTier.remove(cID);

	closesocket(mClients[cID].mSocket);
	mClients[cID].mState = eSessionState::ST_FREE;
	mClients[cID].mStateLock.unlock();

	LD_LOGOUT_PACKET p;
	p.size = sizeof(LD_LOGOUT_PACKET);
	p.type = LD_LOGOUT;
	p.uid = mClients[cID].mUID;

	mServers[0].ServerDoSend(&p);
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
	for (int i = 0; i < MAX_ROOM; ++i) {
		mRooms[i].mStateLock.lock();
		if (mRooms[i].mState == eRoomState::RS_FREE) {
			mRooms[i].mState = eRoomState::RS_READY;
			mRooms[i].mStateLock.unlock();
			return i;
		}
		mRooms[i].mStateLock.unlock();
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
		sp.department = rp->department;
		memcpy(sp.id, rp->id, sizeof(rp->id));
		memcpy(sp.password, rp->password, sizeof(rp->password));
		// send to db server
		mServers[0].DoSend(&sp);
		break;
	}
	case CL_MATCH:
	{
		DEBUGMSGONEPARAM("[%s] 플레이어 매치 시작\n", mClients[cID].mID);
		if (mClients[cID].mGrade > 3.5)
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
		DEBUGMSGONEPARAM("[%s] 플레이어 매치 아웃\n", mClients[cID].mID);
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

		DEBUGMSGNOPARAM("game server login\n");

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

		random_device rd;
		mt19937_64 rng(rd());
		uniform_int_distribution<int> rLevel(MIN_LEVEL, MAX_LEVEL);
		int randomlevel = rLevel(rng);
		
		packet.mapLevel = 2;
		

		int uidCount = 0;
		double userGradeSum = 0;
		mRooms[p->roomID].mStateLock.lock();
		mRooms[p->roomID].mState = eRoomState::RS_INGAME;
		mRooms[p->roomID].mStateLock.unlock();
		mRooms[p->roomID].mRoomLevel = randomlevel;
		DEBUGMSGONEPARAM("레벨[%d] 준비완\n", randomlevel);
		vector<int> delPlayerVector;
		for (auto& player : mReadytoGame)
		{
			if (mClients[player].mRoomID == p->roomID)
			{
				strcpy_s(packet.hashs, mClients[player].mHashs);
				mClients[player].DoSend(&packet);
				mClients[player].mState = eSessionState::ST_INGAME;
				userGradeSum += mClients[player].mGrade;

				mRooms[mClients[player].mRoomID].mUID[uidCount] = mClients[player].mUID;
				mRooms[mClients[player].mRoomID].mSockID[uidCount] = player;
				mRooms[mClients[player].mRoomID].mGrade[uidCount] = mClients[player].mGrade;
				uidCount++;
				delPlayerVector.push_back(player);
			}
		}
		mRooms[p->roomID].mUserNum = uidCount;
		mRooms[p->roomID].mGradeAvg = userGradeSum / uidCount;
		for (auto player : delPlayerVector)
		{
			mReadytoGame.remove(player);
		}

		break;
	}
	case GL_ROOM_RESET:
	{
		GL_ROOM_RESET_PACKET* p = reinterpret_cast<GL_ROOM_RESET_PACKET*>(spacket);
		DEBUGMSGONEPARAM("방[%d] 리셋", p->roomID);
		mRooms[p->roomID].mStateLock.lock();
		mRooms[p->roomID].mState = eRoomState::RS_FREE;
		ZeroMemory(mRooms[p->roomID].mUID, sizeof(mRooms[p->roomID].mUID));
		ZeroMemory(mRooms[p->roomID].mSockID, sizeof(mRooms[p->roomID].mSockID));
		ZeroMemory(mRooms[p->roomID].mGrade, sizeof(mRooms[p->roomID].mGrade));
		mRooms[p->roomID].mUserNum = 0;
		mRooms[p->roomID].mGradeAvg = 0;
		mRooms[p->roomID].mUpdateCount = 0;
		mRooms[p->roomID].mStateLock.unlock();
		break;
	}
	case GL_PLAYER_RESULT:
	{
		GL_PLAYER_RESULT_PACKET* p = reinterpret_cast<GL_PLAYER_RESULT_PACKET*>(spacket);
		for (int i = 0; i < mRooms[p->roomID].mUserNum; i++)
		{
			if (mRooms[p->roomID].mUID[i] == p->uID)
			{
				if (mClients[mRooms[p->roomID].mSockID[i]].mUID == mRooms[p->roomID].mUID[i]) // player connected lobby server
				{
					double GradePerRank;
					if (p->retire == true)
					{
						GradePerRank = -5;
					}
					else
					{
						GradePerRank = GRADE_FOR_SCORE[mRooms[p->roomID].mUserNum - MIN_USER][p->rank - 1]; // 등수 가중치
					}
					
					double GAP = mRooms[p->roomID].mGradeAvg - mClients[mRooms[p->roomID].mSockID[i]].mGrade;
					if (GAP < -4)
					{
						GAP = -4;
					}
					double temp = mRooms[p->roomID].mUserNum * GRADE_CON_NUM * (GradePerRank + GAP);
					if (temp < 0)
					{
						temp *= mClients[mRooms[p->roomID].mSockID[i]].mGrade / 5;
					}
					mClients[mRooms[p->roomID].mSockID[i]].mStateLock.lock();
					mClients[mRooms[p->roomID].mSockID[i]].mGrade += temp;
					mClients[mRooms[p->roomID].mSockID[i]].mState = eSessionState::ST_LOBBY;
					mClients[mRooms[p->roomID].mSockID[i]].mStateLock.unlock();
					// to db server update
					LD_UPDATE_GRADE_PACKET packet;
					packet.grade = mClients[mRooms[p->roomID].mSockID[i]].mGrade;
					packet.uid = mClients[mRooms[p->roomID].mSockID[i]].mUID;
					packet.size = sizeof(LD_UPDATE_GRADE_PACKET);
					packet.type = LD_UPDATE_GRADE;
					mServers[0].DoSend(&packet);


					SendMatchResult(mRooms[p->roomID].mSockID[i], p->rank, 0);

					mRooms[p->roomID].mUpdateCount++;
					DEBUGMSGONEPARAM("[%d]플레이어의 점수 ", packet.uid);
					DEBUGMSGONEPARAM("[%f] 로비안끊김\n", packet.grade);
					break;
				}
				else // player disconnected lobby server
				{
					double GradePerRank = GRADE_FOR_SCORE[mRooms[p->roomID].mUserNum - MIN_USER][p->rank - 1]; // 등수 가중치
					if (p->retire = true)
					{
						GradePerRank = -5;
					}
					double GAP = mRooms[p->roomID].mGradeAvg - mRooms[p->roomID].mGrade[i];
					if (GAP < -4)
					{
						GAP = -4;
					}
					double temp = mRooms[p->roomID].mUserNum * GRADE_CON_NUM * (GradePerRank + GAP);
					if (temp < 0)
					{
						temp *= mRooms[p->roomID].mGrade[i] / 5;
					}
					// to db server update
					LD_UPDATE_GRADE_PACKET packet;
					packet.grade = mRooms[p->roomID].mGrade[i];
					packet.uid = mRooms[p->roomID].mUID[i];
					packet.size = sizeof(LD_UPDATE_GRADE_PACKET);
					packet.type = LD_UPDATE_GRADE;
					mServers[0].DoSend(&packet);
					mRooms[p->roomID].mUpdateCount++;
					DEBUGMSGONEPARAM("[%d]플레이어의 점수 ", packet.uid);
					DEBUGMSGONEPARAM("[%f] 로비 끊김\n", packet.grade);
					break;
				}
			}
		}
		break;
	}
	case DL_LOGIN_OK:
	{
		DL_LOGIN_OK_PACKET* p = reinterpret_cast<DL_LOGIN_OK_PACKET*>(spacket);

		DEBUGMSGONEPARAM("[%s] player login ok\n", p->id);

		if (p->connState == TRUE) {
			CheckDuplicateLogin(p->uid);
		}

		mClients[p->userKey].mStateLock.lock();
		mClients[p->userKey].mGrade = p->grade;
		mClients[p->userKey].mDepartment = p->department;
		strcpy_s(mClients[p->userKey].mNickName, p->nickname);
		strcpy_s(mClients[p->userKey].mID, p->id);
		mClients[p->userKey].mPoint = p->point;
		mClients[p->userKey].mUID = p->uid;
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
		DL_LOGIN_FAIL_PACKET* p = reinterpret_cast<DL_LOGIN_FAIL_PACKET*>(spacket);

		DEBUGMSGONEPARAM("player login fail : [%d]\n", p->errorCode);

		LC_LOGIN_FAIL_PACKET pac;
		pac.size = sizeof(LC_LOGIN_FAIL_PACKET);
		pac.type = LC_LOGIN_FAIL;

		mClients[p->userKey].DoSend(&pac);

		break;
	}
	case DL_SIGNUP_OK:
	{
		DL_SIGNUP_OK_PACKET* p = reinterpret_cast<DL_SIGNUP_OK_PACKET*>(spacket);
		SendSignUpOK(p->userKey);
		break;
	}
	case DL_SIGNUP_FAIL:
	{
		DL_SIGNUP_FAIL_PACKET* p = reinterpret_cast<DL_SIGNUP_FAIL_PACKET*>(spacket);
		SendSignUpFail(p->userKey);
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
				DEBUGMSGNOPARAM("Accept Error\n");
			}
			else
			{
				DEBUGMSGONEPARAM("GQCS Error on client [%s]\n", mClients[client_id].mID);
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
					mServers[server_id].mRecvOver.mTargetID = server_id;

					CreateIoCompletionPort(reinterpret_cast<HANDLE>(cSocket), mHCP, server_id, 0);
					mServers[server_id].DoRecv();
					cSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
					DEBUGMSGNOPARAM("server connect\n");
				}
				else
				{
					DEBUGMSGNOPARAM("Max server exceeded.\n");
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
					DEBUGMSGNOPARAM("player connect\n");
				}
				else
				{
					DEBUGMSGNOPARAM("Max user exceeded.\n");
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
	inet_pton(AF_INET, DBSERVERIP, &serverAddr.sin_addr);

	if (connect(LDsocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		DEBUGMSGNOPARAM("DBconnection failed\n");
	}
	else {
		DEBUGMSGNOPARAM("DBconnection success\n");
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
			DEBUGMSGNOPARAM("server connect\n");
		}
		else
		{
			DEBUGMSGNOPARAM("Max server exceeded.\n");
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
		DEBUGMSGONEPARAM("상위 티어 매칭 인원 [%d]\n", mMatchListHighTier.size());
		DEBUGMSGONEPARAM("하위 티어 매칭 인원 [%d]\n", mMatchListLowTier.size());
		// match room max
		mHighListLock.lock();
		if (mMatchListHighTier.size() >= MAX_ROOM_USER)
		{
			int roomID = GetNewRoomID();
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int player_id = mMatchListHighTier.front();
				DEBUGMSGONEPARAM("[%s]플레이어 게임서버로\n",mClients[player_id].mID);
				LG_USER_INTO_GAME_PACKET packet;
				packet.size = sizeof(packet);
				packet.type = LG_USER_INTO_GAME;
				packet.department = mClients[player_id].mDepartment;
				packet.roomID = roomID;// need update		
				_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10); 
				strcpy_s(packet.hashs, mClients[player_id].mHashs);
				strcpy_s(packet.name,  mClients[player_id].mNickName);
				strcpy_s(packet.id,  mClients[player_id].mID);
				packet.uID = mClients[player_id].mUID;
				packet.roomMax = MAX_ROOM_USER;
				mClients[player_id].mRoomID = packet.roomID;

				mServers[1].ServerDoSend(&packet);

				mMatchListHighTier.pop_front();
				mReadytoGame.push_back(player_id);
			}
		}
		mHighListLock.unlock();
		mLowListlock.lock();
		if (mMatchListLowTier.size() >= MAX_ROOM_USER)
		{
			int roomID = GetNewRoomID();
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int player_id = mMatchListLowTier.front();

				DEBUGMSGONEPARAM("[%s]플레이어 게임서버로\n", mClients[player_id].mID);
				LG_USER_INTO_GAME_PACKET packet;
				packet.size = sizeof(packet);
				packet.type = LG_USER_INTO_GAME;
				packet.roomID = roomID;// need update
				packet.department = mClients[player_id].mDepartment;
				_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10);
				strcpy_s(packet.hashs, mClients[player_id].mHashs);
				strcpy_s(packet.name,  mClients[player_id].mNickName);
				strcpy_s(packet.id,  mClients[player_id].mID);
				packet.uID = mClients[player_id].mUID;
				packet.roomMax = MAX_ROOM_USER;
				mClients[player_id].mRoomID = packet.roomID;

				mServers[1].ServerDoSend(&packet);
				
				mMatchListLowTier.pop_front();
				mReadytoGame.push_back(player_id);
			}
		}
		mLowListlock.unlock();

		// half room max
		system_clock::time_point tTime = system_clock::now();
		mHighListLock.lock();
		if (mMatchListHighTier.size() >= MAX_ROOM_USER / 2) // high list
		{
			if (tTime - mClients[mMatchListHighTier.front()].mMatchStartTime >= milliseconds(20000))
				{
					int tSize = mMatchListHighTier.size();
					int roomID = GetNewRoomID();
					for (int i = 0; i < tSize; ++i)
					{
						int player_id = mMatchListHighTier.front();

						DEBUGMSGONEPARAM("[%s]플레이어 게임서버로\n", mClients[player_id].mID);
						LG_USER_INTO_GAME_PACKET packet;
						packet.size = sizeof(packet);
						packet.type = LG_USER_INTO_GAME;
						packet.roomID = roomID;//need update
						packet.department = mClients[player_id].mDepartment;
						_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10);
						strcpy_s(packet.hashs, mClients[player_id].mHashs);
						strcpy_s(packet.name,  mClients[player_id].mNickName);
						strcpy_s(packet.id,  mClients[player_id].mID);
						packet.uID = mClients[player_id].mUID;
						packet.roomMax = tSize;
						mClients[player_id].mRoomID = packet.roomID;

						mServers[1].ServerDoSend(&packet);

						mMatchListHighTier.pop_front();
						mReadytoGame.push_back(player_id);
					}
				}
			else
				{
					// count down packet?
				}
		}
		mHighListLock.unlock();
		mLowListlock.lock();
		if (mMatchListLowTier.size() >= MAX_ROOM_USER / 2)
		{
			if (tTime - mClients[mMatchListLowTier.front()].mMatchStartTime >= milliseconds(20000))
			{
				int tSize = mMatchListLowTier.size();
				int roomID = GetNewRoomID();
				for (int i = 0; i < tSize; ++i)
				{
					int player_id = mMatchListLowTier.front();

					DEBUGMSGONEPARAM("[%s]플레이어 게임서버로\n", mClients[player_id].mID);
					LG_USER_INTO_GAME_PACKET packet;
					packet.size = sizeof(packet);
					packet.type = LG_USER_INTO_GAME;
					packet.roomID = roomID;//need update
					packet.department = mClients[player_id].mDepartment;
					strcpy_s(packet.name,  mClients[player_id].mNickName);
					strcpy_s(packet.id,  mClients[player_id].mID);
					packet.uID = mClients[player_id].mUID;
					_itoa_s(rand() % 2'000'000'000, mClients[player_id].mHashs, 10);
					strcpy_s(packet.hashs, mClients[player_id].mHashs);
					packet.roomMax = tSize;
					mClients[player_id].mRoomID = packet.roomID;

					mServers[1].ServerDoSend(&packet);

					mMatchListLowTier.pop_front();
					mReadytoGame.push_back(player_id);
				}
			}
			else
			{
				// count down packet?
			}
		}
		mLowListlock.unlock();


		EV_UpdateMatchPacket p;
		p.size = sizeof(EV_UpdateMatchPacket);
		p.type = eCompType::OP_EVENT;
		pTimer->PushEvent(1, eEVENT_TYPE::EV_MATCH_UP, 5000, reinterpret_cast<unsigned char*>(&p));

		break;
	}
	case eEVENT_TYPE::EV_CONTROL:
	{
		cout << " control\n";
		for (int i = 0; i < MAX_USER; i++) {
			if (mClients[i].mState == eSessionState::ST_FREE)
			{
				continue;
			}

			LC_CONTROL_PACKET p;
			p.size = sizeof(LC_CONTROL_PACKET);
			p.type = LC_CONTROL;
			mClients[i].DoSend(&p);
		}


		EV_ControlPacket p;
		p.size = sizeof(EV_ControlPacket);
		p.type = eCompType::OP_EVENT;
		pTimer->PushEvent(1, eEVENT_TYPE::EV_CONTROL, 5000, reinterpret_cast<unsigned char*>(&p));
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

void Server::SendMatchResult(int key, int rank, int point)
{
	LC_GAME_RESULT_PACKET packet;
	packet.grade = mClients[key].mGrade;
	packet.rank = rank;
	packet.point = point;
	packet.size = sizeof(packet);
	packet.type = LC_GAME_RESULT;
	mClients[key].DoSend(&packet);
}

void Server::SendSignUpOK(int key)
{
	LC_SIGNUP_OK_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_SIGNUP_OK;
	mClients[key].DoSend(&packet);
}

void Server::SendSignUpFail(int key)
{
	LC_SIGNUP_FAIL_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_SIGNUP_FAIL;
	mClients[key].DoSend(&packet);
}
