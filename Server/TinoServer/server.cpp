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
		SendLogin(cID, p->id, p->password, p->gameVersion);
		break;
	}
	case CL_LOGOUT:
	{
		CL_LOGOUT_PACKET* p = reinterpret_cast<CL_LOGOUT_PACKET*>(cpacket);
		LD_LOGOUT_PACKET pac;
		pac.type = LD_LOGOUT;
		pac.size = sizeof(LD_LOGOUT_PACKET);
		pac.uid = mClients[cID].mUID;
		mServers[0].DoSend(&pac);
		break;
	}
	case CL_SIGNUP:
	{
		CL_SIGNUP_PACKET* rp = reinterpret_cast<CL_SIGNUP_PACKET*>(cpacket);
		SendSignUp(cID, rp->id, rp->password, rp->department);
		break;
	}
	case CL_MATCH:
	{
		PlayerMatchIn(cID);
		break;
	}
	case CL_MATCH_OUT:
	{
		PlayerMatchOut(cID);
		break;
	}
	case CL_EQUIP_ITEM:
	{
		CL_EQUIP_ITEM_PACKET* p = reinterpret_cast<CL_EQUIP_ITEM_PACKET*>(cpacket);

		if (CheckAbleEquipItem(cID, p->itemCode) == true) {
			SendEquipItem(cID, p->itemCode);
		}
		break;
	}
	case CL_UNEQUIP_ITEM:
	{
		CL_UNEQUIP_ITEM_PACKET* p = reinterpret_cast<CL_UNEQUIP_ITEM_PACKET*>(cpacket);

		if (CheckAbleUnequipItem(cID, p->itemCode) == true) {
			SendUnequipItem(cID, p->itemCode);
		}
		break;
	}
	case CL_BUY_ITEM:
	{
		CL_BUY_ITEM_PACKET* p = reinterpret_cast<CL_BUY_ITEM_PACKET*>(cpacket);
		BuyItem(cID, p->itemCode);
		break;
	}
	case CL_REFRESH_INVENTORY:
	{
		SendInventory(cID);
		break;
	}
	case CL_REFRESH_DEP_RANK: {
		SendRefreshRankingRequest(cID);
		break;
	}
	case CL_REFRESH_POINT: {
		LD_GET_POINT_PACKET p;
		p.size = sizeof(p);
		p.type = LD_GET_POINT;
		p.userKey = cID;
		p.uid = mClients[cID].mUID;

		mServers[0].DoSend(&p);
		break;
	}
	case ML_ALERT: {
		ML_ALERT_PACKET* p = reinterpret_cast<ML_ALERT_PACKET*>(cpacket);
		wcout << p->alert << endl;
		LC_ALERT_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = LC_ALERT;
		memcpy(packet.alert, p->alert, sizeof(packet.alert));
		for (int i = MAXGAMESERVER; i < MAX_USER; ++i)
		{
			mClients[i].mStateLock.lock();
			if (mClients[i].mState == eSessionState::ST_FREE || mClients[i].mState == eSessionState::ST_ACCEPTED)
			{
				mClients[i].mStateLock.unlock();
				continue;
			}
			mClients[i].DoSend(&packet);
			mClients[i].mStateLock.unlock();
		}
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
		DEBUGMSGNOPARAM("game server login\n");
		break;
	}
	case GL_ROOM_READY:
	{
		GL_ROOM_READY_PACKET* p = reinterpret_cast<GL_ROOM_READY_PACKET*>(spacket);
		SendMatchResponse(p->roomID);
		break;
	}
	case GL_ROOM_RESET:
	{
		GL_ROOM_RESET_PACKET* p = reinterpret_cast<GL_ROOM_RESET_PACKET*>(spacket);
		RoomReset(p->roomID);
		break;
	}
	case GL_PLAYER_RESULT:
	{
		GL_PLAYER_RESULT_PACKET* p = reinterpret_cast<GL_PLAYER_RESULT_PACKET*>(spacket);
		SendPlayerResult(p->uID, p->roomID, p->retire, p->rank);
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
		strcpy_s(mClients[p->userKey].mID, p->id);
		mClients[p->userKey].mPoint = p->point;
		mClients[p->userKey].mUID = p->uid;
		mClients[p->userKey].mState = eSessionState::ST_LOBBY;
		mClients[p->userKey].mEquippedItems = p->equipmentflag;
		mClients[p->userKey].mInventory = p->inventoryflag;
		mClients[p->userKey].mStateLock.unlock();

		SendLoginOK(p->userKey, p->ranking);

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
	case DL_BUYITEM_OK:
	{
		DL_BUYITEM_OK_PACKET* p = reinterpret_cast<DL_BUYITEM_OK_PACKET*>(spacket);
		mClients[p->userKey].mPoint = p->pointAfterPurchase;
		AddItemToInventory(p->userKey, p->itemCode);
		SendBuyOK(p->userKey, p->itemCode);
		break;
	}
	case DL_BUYITEM_FAIL:
	{
		DL_BUYITEM_FAIL_PACKET* p = reinterpret_cast<DL_BUYITEM_FAIL_PACKET*>(spacket);
		SendBuyFail(p->userKey);
		break;
	}
	case DL_REFRESH_INVENTORY:
	{
		DL_REFRESH_INVENTORY_PACKET* p = reinterpret_cast<DL_REFRESH_INVENTORY_PACKET*>(spacket);
		// 일단 클라의 인벤토리 새로고침 요청을 로비에서 저장된 데이터로 보내게 해서 안씀
		// 나중에 수정할지 말지 정해야함
		break;
	}
	case DL_REFRESH_DEP_RANK: {
		DL_REFRESH_DEP_RANK_PACKET* p = reinterpret_cast<DL_REFRESH_DEP_RANK_PACKET*>(spacket);
		SendRankingToClient(p->userKey, p->ranking);
		break;
	}
	case DL_EQUIP_OK: {
		DL_EQUIP_OK_PACKET* p = reinterpret_cast<DL_EQUIP_OK_PACKET*>(spacket);
		mClients[p->userKey].mEquippedItems = p->equipmentFlag;
		SendEquipItemOK(p->userKey, p->itemCode, p->equipmentFlag);
		break;
	}
	case DL_UNEQUIP_OK: {
		DL_UNEQUIP_OK_PACKET* p = reinterpret_cast<DL_UNEQUIP_OK_PACKET*>(spacket);
		mClients[p->userKey].mEquippedItems = p->equipmentFlag;
		SendUnequipItemOK(p->userKey, p->itemCode, p->equipmentFlag);
		break;
	}
	case DL_GET_POINT: {
		DL_GET_POINT_PACKET* p = reinterpret_cast<DL_GET_POINT_PACKET*>(spacket);
	
		LC_REFRESH_POINT_PACKET pac;
		pac.point = p->point;
		pac.size = sizeof(pac);
		pac.type = LC_REFRESH_POINT;

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

	// Item Data Load
	LoadGameData();

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
	TimerEvent* te = reinterpret_cast<TimerEvent*>(cmessage);

	switch (te->mEventType) {
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
				packet.equipmentflag = mClients[player_id].mEquippedItems;
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
				packet.equipmentflag = mClients[player_id].mEquippedItems;
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
		if (mMatchListHighTier.size() >= MAX_ROOM_USER / 2 && mMatchListHighTier.size() < MAX_ROOM_USER) // high list
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
						packet.equipmentflag = mClients[player_id].mEquippedItems;
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
		if (mMatchListLowTier.size() >= MAX_ROOM_USER / 2 && mMatchListLowTier.size() < MAX_ROOM_USER)
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
					packet.equipmentflag = mClients[player_id].mEquippedItems;
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
		pTimer->PushEvent(1, eEVENT_TYPE::EV_MATCH_UP, 1000, reinterpret_cast<unsigned char*>(&p));

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

void Server::PlayerMatchIn(int cID)
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
}

void Server::PlayerMatchOut(int cID)
{
	DEBUGMSGONEPARAM("[%s] 플레이어 매치 아웃\n", mClients[cID].mID);
	mHighListLock.lock();
	mMatchListHighTier.remove(cID);
	mHighListLock.unlock();
	mLowListlock.lock();
	mMatchListLowTier.remove(cID);
	mLowListlock.unlock();

	if (mMatchListHighTier.size() >= MAX_ROOM_USER / 2 && mMatchListHighTier.size() < MAX_ROOM_USER)
	{
		mClients[mMatchListHighTier.front()].mMatchStartTime = system_clock::now();
	}
	if (mMatchListLowTier.size() >= MAX_ROOM_USER / 2 && mMatchListLowTier.size() < MAX_ROOM_USER)
	{
		mClients[mMatchListLowTier.front()].mMatchStartTime = system_clock::now();
	}
	mClients[cID].mState = eSessionState::ST_LOBBY;
}

void Server::RoomReset(int roomID)
{
	DEBUGMSGONEPARAM("방[%d] 리셋", roomID);
	mRooms[roomID].mStateLock.lock();
	mRooms[roomID].mState = eRoomState::RS_FREE;
	ZeroMemory(mRooms[roomID].mUID, sizeof(mRooms[roomID].mUID));
	ZeroMemory(mRooms[roomID].mSockID, sizeof(mRooms[roomID].mSockID));
	ZeroMemory(mRooms[roomID].mGrade, sizeof(mRooms[roomID].mGrade));
	mRooms[roomID].mUserNum = 0;
	mRooms[roomID].mGradeAvg = 0;
	mRooms[roomID].mUpdateCount = 0;
	mRooms[roomID].mStateLock.unlock();
}

void Server::BuyItem(int cID, int itemCode)
{
	if ((mClients[cID].mInventory & (1LL << itemCode)) != 0) {
		SendBuyFail(cID);
		return;
	}

	if (mClients[cID].mPoint - pGameDataManager->GetShopProductInfo(itemCode).price < 0) {
		SendBuyFail(cID);
		return;
	}
	else {
		SendBuyItem(cID, itemCode);
	}
}

void Server::AddItemToInventory(int cID, int itemCode)
{
	mClients[cID].mInventory = mClients[cID].mInventory | (1ULL << itemCode);
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

bool Server::CheckAbleEquipItem(int cID, int itemCode)
{
	long long result = mClients[cID].mInventory & (1LL << itemCode);
	if (result == 0) return false;

	result = mClients[cID].mEquippedItems & (1LL << itemCode);
	if (result == 0) return true;
	return false;
}

bool Server::CheckAbleUnequipItem(int cID, int itemCode)
{
	long long result = mClients[cID].mInventory & (1LL << itemCode);
	if (result == 0) return false;

	result = mClients[cID].mEquippedItems & (1LL << itemCode);
	if (result == 0) return false;

	return true;
}

void Server::SendPlayerResult(int uID, int roomID, bool retire, int rank)
{
	for (int i = 0; i < mRooms[roomID].mUserNum; i++)
	{
		if (mRooms[roomID].mUID[i] == uID)
		{
			if (mClients[mRooms[roomID].mSockID[i]].mUID == mRooms[roomID].mUID[i]) // player connected lobby server
			{
				// grade system
				double GradePerRank;
				if (retire == true)
				{
					GradePerRank = -3;
				}
				else
				{
					GradePerRank = GRADE_FOR_SCORE[mRooms[roomID].mUserNum - MIN_USER][rank - 1]; // 등수 가중치
				}

				double GAP = mRooms[roomID].mGradeAvg - mClients[mRooms[roomID].mSockID[i]].mGrade;
				if (GAP < -4)
				{
					GAP = -4;
				}
				double temp = mRooms[roomID].mUserNum * GRADE_CON_NUM * (GradePerRank + GAP);
				if (temp < 0)
				{
					temp *= mClients[mRooms[roomID].mSockID[i]].mGrade / 5;
				}

				// point system
				int point;
				if (retire == true)
				{
					if (rank == -1) // 탈주
					{
						point = 0;
					}
					else // 그냥 완주 못함
					{
						point = 50;
					}
				}
				else
				{
					point = POINT_FOR_SCORE[mRooms[roomID].mUserNum - MIN_USER][rank - 1];
				}

				mClients[mRooms[roomID].mSockID[i]].mStateLock.lock();
				mClients[mRooms[roomID].mSockID[i]].mGrade += temp;
				mRooms[roomID].mGrade[i] = mClients[mRooms[roomID].mSockID[i]].mGrade;
				mClients[mRooms[roomID].mSockID[i]].mPoint = point;
				mRooms[roomID].mPoint[i] = mClients[mRooms[roomID].mSockID[i]].mPoint;
				mClients[mRooms[roomID].mSockID[i]].mState = eSessionState::ST_LOBBY;
				mClients[mRooms[roomID].mSockID[i]].mStateLock.unlock();
				// to db server update
				SendGameResult(roomID, i);
				SendMatchResult(mRooms[roomID].mSockID[i], rank, mRooms[roomID].mPoint[i]);
				DEBUGMSGONEPARAM("[%d]플레이어의 점수 : ", mClients[mRooms[roomID].mSockID[i]].mUID);
				DEBUGMSGONEPARAM("[%f] 로비안끊김    플레이어의 포인트 : ", mClients[mRooms[roomID].mSockID[i]].mGrade);
				DEBUGMSGONEPARAM("[%d] 로비안끊김\n", mClients[mRooms[roomID].mSockID[i]].mPoint);
				break;
			}
			else // player disconnected lobby server
			{
				// grade system
				double GradePerRank = GRADE_FOR_SCORE[mRooms[roomID].mUserNum - MIN_USER][rank - 1]; // 등수 가중치
				if (retire = true)
				{
					GradePerRank = -3;
				}
				double GAP = mRooms[roomID].mGradeAvg - mRooms[roomID].mGrade[i];
				if (GAP < -4)
				{
					GAP = -4;
				}
				double temp = mRooms[roomID].mUserNum * GRADE_CON_NUM * (GradePerRank + GAP);
				if (temp < 0)
				{
					temp *= mRooms[roomID].mGrade[i] / 5;
				}

				// point system
				int point;
				if (retire == true)
				{
					if (rank == -1) // 탈주
					{
						point = 0;
					}
					else // 그냥 완주 못함
					{
						point = 50;
					}
				}
				else
				{
					point = POINT_FOR_SCORE[mRooms[roomID].mUserNum - MIN_USER][rank - 1];
				}


				mRooms[roomID].mGrade[i] += temp;
				mRooms[roomID].mPoint[i] = point;
				// to db server update
				SendGameResult(roomID, i);
				DEBUGMSGONEPARAM("[%d]플레이어의 점수 : ", mRooms[roomID].mUID[i]);
				DEBUGMSGONEPARAM("[%f] 로비 끊김  플레이어의 포인트 : ", mRooms[roomID].mGrade[i]);
				DEBUGMSGONEPARAM("[%d] 로비 끊김\n", mRooms[roomID].mPoint[i]);
				break;
			}
		}
	}
}

void Server::SendLogin(int cID, char* id, char* pass, char* version)
{
	LD_LOGIN_PACKET pac;
	pac.type = LD_LOGIN;
	pac.size = sizeof(LD_LOGIN_PACKET);
	pac.userKey = cID;
	memcpy(pac.id, id, sizeof(pac.id));
	memcpy(pac.password, pass, sizeof(pac.password));
	memcpy(pac.gameVersion, version, sizeof(pac.gameVersion));
	mServers[0].DoSend(&pac);
}

void Server::SendSignUp(int cID, char* id, char* pass, char depart)
{
	LD_SIGNUP_PACKET sp;
	sp.size = sizeof(sp);
	sp.type = LD_SIGNUP;
	sp.department = depart;
	sp.userKey = cID;
	memcpy(sp.id, id, sizeof(sp.id));
	memcpy(sp.password, pass, sizeof(sp.password));
	mServers[0].DoSend(&sp);
}

void Server::SendGameResult(int roomID, int key)
{
	LD_UPDATE_GRADE_PACKET packet;
	packet.grade = mRooms[roomID].mGrade[key];
	packet.uid = mRooms[roomID].mUID[key];
	packet.point = mRooms[roomID].mPoint[key];
	packet.size = sizeof(LD_UPDATE_GRADE_PACKET);
	packet.type = LD_UPDATE_GRADE;
	mServers[0].DoSend(&packet);
}

void Server::SendEquipItem(int cID, int itemCode)
{
	LD_EQUIP_ITEM_PACKET packet;

	packet.size = sizeof(LD_EQUIP_ITEM_PACKET);
	packet.type = LD_EQUIP_ITEM;
	packet.uid = mClients[cID].mUID;
	packet.itemCode = itemCode;
	packet.userKey = cID;

	mServers[0].DoSend(&packet);
}

void Server::SendUnequipItem(int cID, int itemCode)
{
	LD_UNEQUIP_ITEM_PACKET packet;

	packet.size = sizeof(LD_UNEQUIP_ITEM_PACKET);
	packet.type = LD_UNEQUIP_ITEM;
	packet.uid = mClients[cID].mUID;
	packet.itemCode = itemCode;
	packet.userKey = cID;

	mServers[0].DoSend(&packet);
}

void Server::SendBuyItem(int cID, int itemCode)
{
	LD_BUY_ITEM_PACKET packet;

	packet.size = sizeof(LD_EQUIP_ITEM_PACKET);
	packet.type = LD_BUY_ITEM;
	packet.uid = mClients[cID].mUID;
	packet.itemCode = itemCode;
	packet.price = pGameDataManager->GetShopProductInfo(itemCode).price;
	packet.userKey = cID;

	mServers[0].DoSend(&packet);
}

void Server::SendRefreshRankingRequest(int cID)
{
	LD_REFRESH_DEP_RANK_PACKET packet;
	packet.size = sizeof(LD_REFRESH_DEP_RANK_PACKET);
	packet.type = LD_REFRESH_DEP_RANK;
	packet.userKey = cID;

	mServers[0].DoSend(&packet);
}


void Server::SendLoginOK(int cID,const rankInfo* rank)
{
	LC_LOGIN_OK_PACKET pac;
	pac.type = LC_LOGIN_OK;
	pac.size = sizeof(LC_LOGIN_OK_PACKET);
	pac.id = cID;
	pac.grade = mClients[cID].mGrade;
	pac.point = mClients[cID].mPoint;
	pac.RoomID = 0;
	pac.UID = mClients[cID].mUID;
	pac.equippedItems = mClients[cID].mEquippedItems;
	pac.inventoryFlag = mClients[cID].mInventory;
	memcpy(pac.ranking, rank, sizeof(pac.ranking));
	mClients[cID].DoSend(&pac);
}

void Server::SendDiconnectPacketToGameServer(int key, int uid, int roomID)
{
	LG_USER_DISCONNECT_PACKET p;
	p.size = sizeof(p);
	p.type = LG_USER_DISCONNECT;
	p.uID = uid;
	p.roomID = roomID;
	
	mServers[1].DoSend(reinterpret_cast<char*>(&p));
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

void Server::SendMatchResponse(int roomID)
{
	LC_MATCH_RESPONSE_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_MATCH_RESPONSE;
	memcpy(packet.gameServerIP, SERVERIP, sizeof(packet.gameServerIP));
	packet.gameServerPortNum = GAMESERVERPORT;

	random_device rd;
	mt19937_64 rng(rd());
	uniform_int_distribution<int> rLevel(MIN_LEVEL, MAX_LEVEL);
	int randomlevel = rLevel(rng);

	packet.mapLevel = randomlevel;


	int uidCount = 0;
	double userGradeSum = 0;
	mRooms[roomID].mStateLock.lock();
	mRooms[roomID].mState = eRoomState::RS_INGAME;
	mRooms[roomID].mStateLock.unlock();
	mRooms[roomID].mRoomLevel = randomlevel;
	DEBUGMSGONEPARAM("레벨[%d] 준비완\n", randomlevel);
	vector<int> delPlayerVector;
	for (auto& player : mReadytoGame)
	{
		if (mClients[player].mRoomID == roomID)
		{
			strcpy_s(packet.hashs, mClients[player].mHashs);
			mClients[player].DoSend(&packet);
			mClients[player].mState = eSessionState::ST_INGAME;
			userGradeSum += mClients[player].mGrade;

			mRooms[mClients[player].mRoomID].mUID[uidCount] = mClients[player].mUID;
			mRooms[mClients[player].mRoomID].mSockID[uidCount] = player;
			mRooms[mClients[player].mRoomID].mGrade[uidCount] = mClients[player].mGrade;
			mRooms[mClients[player].mRoomID].mPoint[uidCount] = mClients[player].mPoint;
			uidCount++;
			delPlayerVector.push_back(player);
		}
	}
	mRooms[roomID].mUserNum = uidCount;
	mRooms[roomID].mGradeAvg = userGradeSum / uidCount;
	for (auto player : delPlayerVector)
	{
		mReadytoGame.remove(player);
	}
}

void Server::SendBuyOK(int key, int itemCode)
{
	LC_BUYITEM_OK_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_BUYITEM_OK;
	packet.pointAfterPurchase = mClients[key].mPoint;
	packet.itemCode = itemCode;
	packet.inventoryFlag = mClients[key].mInventory;
	mClients[key].DoSend(&packet);
}

void Server::SendBuyFail(int key)
{
	LC_BUYITEM_FAIL_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_BUYITEM_FAIL;
	mClients[key].DoSend(&packet);
}

void Server::SendInventory(int key)
{
	LC_REFRESH_INVENTORY_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_REFRESH_INVENTORY;
	packet.inventoryFlag = mClients[key].mInventory;
	mClients[key].DoSend(&packet);
}

void Server::SendRankingToClient(int key, rankInfo* ranking)
{
	LC_REFRESH_DEP_RANK_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_REFRESH_DEP_RANK;
	for (int i = 0; i < 10; ++i) {
		packet.ranking[i] = ranking[i];
	}
	mClients[key].DoSend(&packet);
}

void Server::SendEquipItemOK(int key, int itemCode, long long equipmentFlag)
{
	LC_EQUIP_OK_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_EQUIP_OK;
	packet.itemCode = itemCode;
	packet.equipmentFlag = equipmentFlag;
	mClients[key].DoSend(&packet);
}

void Server::SendUnequipItemOK(int key, int itemCode, long long equipmentFlag)
{
	LC_UNEQUIP_OK_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = LC_UNEQUIP_OK;
	packet.itemCode = itemCode;
	packet.equipmentFlag = equipmentFlag;
	mClients[key].DoSend(&packet);
}


void Server::LoadGameData()
{
	pGameDataManager = new GameDataManager;
	bool result = pGameDataManager->LoadItemData();
	if (result == false) {
		DEBUGMSGNOPARAM("Load Item Data Failed\n");
		return;
	}
	result = pGameDataManager->LoadShopData();
	if (result == false) {
		DEBUGMSGNOPARAM("Load Shop Data Failed\n");
		return;
	}

	if (result == true) {
		DEBUGMSGNOPARAM("Load Game Data Succeed\n");
	}
}
