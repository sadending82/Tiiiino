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
		pac.user_id = cID;
		memcpy(pac.id, p->id, sizeof(pac.id));
		memcpy(pac.password, p->password, sizeof(pac.password));
		cout << pac.id << endl;
		// db 서버에 전송
		mServers[0].DoSend(&pac);
		break;
	}
	case CL_MATCH:
	{
		CL_MATCH_PACKET* p = reinterpret_cast<CL_MATCH_PACKET*>(cpacket);
		/*
		지금은!게임서버한테 바로 보내주면 됨.
		나중엔 이 패킷에서 보내주는게 아니라 여기로 들어온 플레이어들을 모아서
		따로 매칭 로직을 돌린 후에 매칭이 성사 되면 그 함수에서 아래의 패킷을 보내주면 됨.
		그리고 이 패킷 하나 크기가 46이라서 8명을 한꺼번에 보내면 344바이트임. 한번에 못보냄
		포문 돌려서 인원수만큼 보내주면 됨.
		
		LG_USER_INTO_GAME_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = LG_USER_INTO_GAME;
		packet.roomID = 0; //여기에 Room Number인데~ 이건 이제 로비서버에서 매칭 로직 돌리면서 정해줘야함 지금은 0 넣으면 됨.
		strcpy(packet.name, mClients[cID].GetName());
		strcpy(packet.passWord, mClients[cID].GetPassWord());
		packet.roomMax = 8;	//여기도 몇명이서 진행하는지 넣는 값. 테스트에는 거의 8명이서 할 거니까 8을 넣어준다. 4명이서 하면 4를 넣는다.
		sendToGameServer(packet);
		*/

		LG_USER_INTO_GAME_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = LG_USER_INTO_GAME;
		packet.roomID = 0;//방 번호 임시로 0으로 넣어둠
		strcpy_s(packet.name, sizeof(mClients[cID].mNickName), mClients[cID].mNickName);
		packet.uID = mClients[cID].mUID;
		packet.roomMax = 1;
		mClients[cID].mRoomID = packet.roomID;	
		mServers[1].DoSend(&packet);

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

		cout << "게임 서버 접속 확인" << endl;

		break;
	}
	case GL_ROOM_READY:
	{
		GL_ROOM_READY_PACKET* p = reinterpret_cast<GL_ROOM_READY_PACKET*>(spacket);
		LC_MATCH_RESPONSE_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = LC_MATCH_RESPONSE;
		for (auto& player : mClients)
		{
			if (player.mRoomID == p->roomID)
			{
				player.DoSend(&packet);
			}
		}
		//자리에 없으셔서 만든 비효율적인 코드 나중에 고쳐주십쇼
		

		/*
			패킷의 roomID로 room을 준비 완료로 바꾸고, 클라이언트들에게 게임서버로 가라는 패킷을 보냄.
			LC_MATCH_RESPONSE_PACKET packet;
			packet.size = sizeof(packet);
			packet.type = LC_MATCH_RESPONSE;
			packet.gameServerPortNum = GAMESERVERPORT + n;	//나중에 게임서버 여러개까지 고려
			strcpy(packet.gameServerIP,"127.0.0.1"대충아이피);
			sendToClient(packet);

		*/

		break;
	}
	case DL_LOGIN_OK:
	{
		cout << "로그인 성공" << endl;

		DL_LOGIN_OK_PACKET* p = reinterpret_cast<DL_LOGIN_OK_PACKET*>(spacket);
		mClients[p->user_id].mCredit = p->credit;
		strcpy_s(mClients[p->user_id].mNickName, sizeof(p->nickname), p->nickname);
		mClients[p->user_id].mPoint = p->point;
		mClients[p->user_id].mUID = p->uid;

		LC_LOGIN_OK_PACKET pac;
		pac.type = LC_LOGIN_OK;
		pac.size = sizeof(LC_LOGIN_OK_PACKET);
		pac.id = p->user_id;
		pac.RoomID = 0;
		pac.UID = mClients[p->user_id].mUID;

		mClients[p->user_id].DoSend(&pac);

		// 클라쪽에 로그인 성공 했다고 알려줘야 함

		break;
	}
	case DL_LOGIN_FAIL:
	{
		cout << "로그인 실패" << endl;
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
	/*EV_UpdateMatchPacket p;
	p.size = sizeof(EV_UpdateMatchPacket);
	p.type = 0;

	pTimer->PushEvent(1, eEVENT_TYPE::EV_MATCH_UP, 5000, reinterpret_cast<char*>(&p));
	*/

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
					mServers[server_id].mPlayerID = server_id;
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
				//게임서버가 더 켜져야 한다면
				/*
				if(mGameServerCnt < mMaxGameServerCnt) //<<두개 변수 추가해주고
					AcceptEx(mListenSocket, cSocket, exOver->mMessageBuf, BUF_SIZE - 8, addr_size + 16, addr_size + 16, 0, &exOver->mOver);
				else
				*/
				AcceptEx(mListenSocket, cSocket, exOver->mMessageBuf, 0, addr_size + 16, addr_size + 16, 0, &exOver->mOver);
			}
			else {
				int client_id = GetNewClientID();
				if (client_id != INVALID_KEY)
				{
					mClients[client_id].mPlayerID = client_id;
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
		default :
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
		cout << "DB서버 커넥트 실패" << endl;
	}
	else {
		cout << "DB서버 커넥트 성공" << endl;
		OverEXP ss_over;
		ss_over.mCompType = eCompType::OP_ACCEPT;
		ss_over.mWsaBuf.buf = reinterpret_cast<CHAR*>(LDsocket);

		// 바인드 걸어 주기 -> 기존 클라 말고 서버 쪽으로

		int server_id = GetNewServerID();
		if (server_id != INVALID_KEY)
		{
			mServers[server_id].mPlayerID = server_id;
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

		//ZeroMemory(&ss_over.mOver, sizeof(ss_over.mOver));
		//ss_over.mWsaBuf.buf = reinterpret_cast<CHAR*>(LDsocket);
		//int addr_size = sizeof(SOCKADDR_IN);
		//AcceptEx(mListenSocket, LDsocket, ss_over.mMessageBuf, 0, addr_size + 16, addr_size + 16, 0, &ss_over.mOver);
		//
		//LD_LOGIN_PACKET p;
		//p.size = sizeof(LD_LOGIN_PACKET);
		//p.type = LD_LOGIN;
		////p.testNum = 123;
		//
		//mServers[server_id].DoSend(&p);
	}

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
		mWorkerThreads.emplace_back(thread(&Server::DoWorker, this));

	pTimer = new Timer;
	pTimer->Init(mHCP);
	delete pTimer;

	for (auto& th : mWorkerThreads)
		th.join();

	closesocket(mListenSocket);
	WSACleanup();
}

void Server::ProcessEvent(unsigned char* cmessage)
{
	switch (cmessage[1]) {
	case static_cast<int> (eEVENT_TYPE::EV_MATCH_UP):
	{
		cout << "작동 확인 0" << endl;
		break;
	}
	case static_cast<int> (eEVENT_TYPE::EV_MATCH_IN):
	{
		cout << "작동 확인 1" << endl;
		break;
	}
	case static_cast<int> (eEVENT_TYPE::EV_MATCH_OUT):
	{
		cout << "작동 확인 2" << endl;
		break;
	}
	default:
	{
		break;
	}
	}
}