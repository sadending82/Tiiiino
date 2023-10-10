#include <fstream>
#include "MainServer.h"

#include "../LobbyServer/LobbyServer.h"
#include "../../Object/Player/Player.h"
#include "../../Room/Room.h"
#include "../../Thread/TimerThread/TimerThread.h"
#include "../../Thread/WorkerThread/WorkerThread.h"
#include "../../../../../ServerProtocol.h"

MainServer* gMainServer;

using namespace std;

MainServer::MainServer()
{

}

MainServer::~MainServer()
{
	for (auto& object : mObjects) {
		auto player = reinterpret_cast<Player*>(object);
		if (!player) break;
		player->DisConnectAndReset();
	}

	for (auto& object : mObjects)
	{
		if (object)
			delete object;
	}

	for (auto& room : mRooms)
	{
		if (room.second)
			delete room.second;
	}

	delete mWorkerThreadRef;

	closesocket(mSocket);
	WSACleanup();
}


void MainServer::init()
{
	//--------------------------------
	/*

		CPU 선호도 정할 코드 넣는 곳.

	*/
	//-------------------------------
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(GAMESERVERPORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(mSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(mSocket, SOMAXCONN);

	int option = TRUE;               //네이글 알고리즘 on/off
	setsockopt(mSocket,             //해당 소켓
		IPPROTO_TCP,          //소켓의 레벨
		TCP_NODELAY,          //설정 옵션
		(const char*)&option, // 옵션 포인터
		sizeof(option));      //옵션 크기

	//std::cout << ntohs(server_addr.sin_port) << std::endl;
	mhiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(mSocket), mhiocp, 0, 0);

	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	char	accept_buf[sizeof(SOCKADDR_IN) * 2 + 32 + 100];
	WSA_OVER_EX	accept_ex;
	*(reinterpret_cast<SOCKET*>(accept_ex.GetBuf())) = c_socket;

	ZeroMemory(&accept_ex.GetWsaOver(), sizeof(accept_ex.GetWsaOver()));
	accept_ex.SetCmd(eCOMMAND_IOCP::CMD_ACCEPT);

	AcceptEx(mSocket, c_socket, accept_buf, 0, sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, NULL, &accept_ex.GetWsaOver());
	std::cout << "Accept Called\n";

	for (auto object : mObjects)
		object = nullptr;
	for (int i = 0; i < MAX_USER; ++i)
	{
		mObjects[i] = new Player();

	}
	for (int i = 0; i < MAX_ROOM; ++i)
	{
		Room* room = new Room(i);
		room->Init();
		mRooms.insert(make_pair(i, room));
	}
	connectLobbyServer();
	mWorkerThreadRef = new WorkerThread(this);
	mTimerThreadRef = new TimerThread();
	std::cout << "Creating Worker Threads\n";
	//vector<thread> worker_threads;
	//thread timer_thread{ TimerThread };
	//thread logger_thread{ LogThread };

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
		mWorkerThreads.emplace_back(&WorkerThread::doThread, mWorkerThreadRef);
	mTimerThread = thread(&TimerThread::doThread, mTimerThreadRef);
}

void MainServer::run()
{
	mTimerThread.join();
	for (auto& th : mWorkerThreads)
		th.join();

}

int MainServer::GenerateID()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		auto user = dynamic_cast<Player*>(mObjects[i]);
		if (user != nullptr)
			if (user->CanMakeID())
				return i;
	}
	std::cout << "Player is Over the MAX_USER" << std::endl;
	return -1;
}

void MainServer::SendPacketWithID(const int receiverID, void* buf, const int bufSize)
{
	auto player = dynamic_cast<Player*>(mObjects[receiverID]);
	if (nullptr != player)
		player->SendPacket(buf, bufSize);
}

void MainServer::send_login_ok_packet(const int player_id, const char* playername)
{
	auto player = dynamic_cast<Player*>(mObjects[player_id]);
	SC_LOGIN_OK_PACKET packet;
	memset(&packet, 0, sizeof(SC_LOGIN_OK_PACKET));

	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;

	packet.id = player_id;
	//wcscpy(packet.name, playername);
	player->SendPacket(&packet, sizeof(packet));
}

void MainServer::send_login_ok_packet(const int player_id, void* buf)
{
	auto player = dynamic_cast<Player*>(mObjects[player_id]);
	SC_LOGIN_OK_PACKET packet = reinterpret_cast<SC_LOGIN_OK_PACKET&>(buf);
	player->SendPacket(&packet, sizeof(packet));
}

SC_LOGIN_OK_PACKET MainServer::make_login_ok_packet(const int playerSocketID, const int playerID, const char* playername)
{
	auto player = dynamic_cast<Player*>(mObjects[playerSocketID]);
	SC_LOGIN_OK_PACKET packet;
	memset(&packet, 0, sizeof(SC_LOGIN_OK_PACKET));

	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;

	packet.id = playerID;
	if(player)
		packet.department = static_cast<char>(player->GetDepartment());
	//wcscpy(packet.name, playername);
	return packet;
}

void MainServer::send_player_add_packet(const int playerID, void* buf, const int bufSize)
{
	auto player = dynamic_cast<Player*>(mObjects[playerID]);
	player->SendPacket(buf, bufSize);

}

SC_ADD_PLAYER_PACKET MainServer::make_player_add_packet(const int playerSocketID)
{
	Player* player = dynamic_cast<Player*>(mObjects[playerSocketID]);
	SC_ADD_PLAYER_PACKET sendpacket;
	sendpacket.id = player->GetRoomSyncID();
	//strcpy_s(sendpacket.name, character->name);
	sendpacket.size = sizeof(sendpacket);
	sendpacket.type = SC_ADD_PLAYER;
	sendpacket.department = static_cast<char>(player->GetDepartment());
	sendpacket.x = player->GetPosition().x;;
	sendpacket.y = player->GetPosition().y;
	sendpacket.z = player->GetPosition().z;
	sendpacket.rx = player->GetRotate().x;
	sendpacket.ry = player->GetRotate().y;
	sendpacket.rz = player->GetRotate().z;
	sendpacket.rw = player->GetRotate().w;
	return sendpacket;
}

SC_PLAYER_REMOVE_PACKET MainServer::make_player_remove_packet(const int playerRoomSyncID)
{
	SC_PLAYER_REMOVE_PACKET sendpacket;
	sendpacket.size = sizeof(sendpacket);
	sendpacket.type = SC_PLAYER_REMOVE;
	sendpacket.id = playerRoomSyncID;
	return sendpacket;
}

void MainServer::send_room_ready_packet(const int roomID)
{
	GL_ROOM_READY_PACKET packet{};
	memset(&packet, 0, sizeof(GL_ROOM_READY_PACKET));

	packet.size = sizeof(packet);
	packet.type = GL_ROOM_READY;
	packet.roomID = roomID;

	mLobbyServer->SendPacket(&packet, sizeof(packet));
}

void MainServer::send_room_reset_packet(const int roomID)
{
	GL_ROOM_RESET_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = GL_ROOM_RESET;
	packet.roomID = roomID;

	mLobbyServer->SendPacket(&packet, sizeof(packet));
}

void MainServer::send_player_result_packet(const int uID, const int rank,const int roomID,const bool retire)
{
	GL_PLAYER_RESULT_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = GL_PLAYER_RESULT;
	packet.uID = uID;
	packet.rank = rank;
	packet.roomID = roomID;
	packet.retire = retire;

	mLobbyServer->SendPacket(&packet, sizeof(packet));
}

void MainServer::send_move_packet(const int player_id, const int mover_id, const bool inair, const float value, const float sx, const float sy, const float sz)
{
	auto player = reinterpret_cast<Player*>(mObjects[player_id]);
	SC_MOVE_PLAYER_PACKET packet{};
	packet.id = mover_id;
	packet.size = sizeof(packet);
	packet.type = SC_MOVE_PLAYER;
	packet.inair = inair;
	packet.x = mObjects[mover_id]->GetPosition().x;
	packet.y = mObjects[mover_id]->GetPosition().y;
	packet.z = mObjects[mover_id]->GetPosition().z;
	packet.rx = mObjects[mover_id]->GetRotate().x;
	packet.ry = mObjects[mover_id]->GetRotate().y;
	packet.rz = mObjects[mover_id]->GetRotate().z;
	packet.rw = mObjects[mover_id]->GetRotate().w;
	auto mover = dynamic_cast<Player*>(mObjects[mover_id]);
	if (mover)
		packet.move_time = mover->GetMoveTime();
	packet.speed = value;
	packet.sx = sx;
	packet.sy = sy;
	packet.sz = sz;
	player->SendPacket(&packet, sizeof(packet));
}

SC_MOVE_PLAYER_PACKET MainServer::make_move_packet(const int moverSocketID, const bool inair, const float value, const float sx, const float sy, const float sz)
{
	SC_MOVE_PLAYER_PACKET packet{};
	auto mover = dynamic_cast<Player*>(mObjects[moverSocketID]);
	packet.id = mover->GetRoomSyncID();
	packet.size = sizeof(packet);
	packet.type = SC_MOVE_PLAYER;
	packet.inair = inair;
	packet.x = mover->GetPosition().x;
	packet.y = mover->GetPosition().y;
	packet.z = mover->GetPosition().z;
	packet.rx = mover->GetRotate().x;
	packet.ry = mover->GetRotate().y;
	packet.rz = mover->GetRotate().z;
	packet.rw = mover->GetRotate().w;
	if (mover)
		packet.move_time = mover->GetMoveTime();
	packet.speed = value;
	packet.sx = sx;
	packet.sy = sy;
	packet.sz = sz;
	return packet;
}

void MainServer::send_player_arrive_packet(const int player_id, const int arrive_id)
{
}

SC_PLAYER_ARRIVE_PACKET MainServer::make_player_arrive_packet(const int arriveID)
{
	SC_PLAYER_ARRIVE_PACKET packet{};
	packet.id = arriveID;
	packet.size = sizeof(packet);
	packet.type = SC_PLAYER_ARRIVE;

	return packet;
}

void MainServer::send_game_countdown_start_packet(const int player_id)
{
}

SC_GAME_COUNTDOWN_START_PACKET MainServer::make_game_countdown_start_packet()
{
	SC_GAME_COUNTDOWN_START_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_COUNTDOWN_START;

	return packet;
}

SC_PING_PACKET MainServer::make_ping_packet()
{
	SC_PING_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_PING;
	packet.ping = static_cast<unsigned>(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
	return packet;
}

SC_ACTION_ANIM_PACKET MainServer::make_action_packet(const int playerID, const char action)
{
	SC_ACTION_ANIM_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_ACTION_ANIM;
	packet.id = playerID;
	packet.action = action;
	return packet;
}

SC_GAME_WAITTING_PACKET MainServer::make_game_watting_packet()
{
	SC_GAME_WAITTING_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_WAITTING;
	return packet;
}

SC_GAME_START_PACKET MainServer::make_game_start_packet()
{
	SC_GAME_START_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_START;
	return packet;
}

SC_GAME_END_PACKET MainServer::make_game_end_packet()
{
	SC_GAME_END_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_END;
	return packet;
}

SC_GAME_DOORSYNC_PACKET MainServer::make_game_doorsync_packet(const int objectID, const long long syncTime)
{
	SC_GAME_DOORSYNC_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_DOORSYNC;
	packet.id = objectID;
	packet.syncTime = syncTime;
	return packet;
}

SC_GAME_PLAYERLOAD_OK_PACKET MainServer::make_game_playerload_ok_packet()
{
	SC_GAME_PLAYERLOAD_OK_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_PLAYERLOAD_OK;
	return packet;
}

SC_GAME_BREAKDOOR_PACKET MainServer::make_game_breakdoor_packet(const int objectID)
{
	SC_GAME_BREAKDOOR_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_BREAKDOOR;
	packet.objectID = objectID;
	return packet;
}

SC_GAME_BREAKPLATFORM_PACKET MainServer::make_game_breakplatform_packet(const int objectID)
{
	SC_GAME_BREAKPLATFORM_PACKET packet{};
	packet.size = sizeof(packet);
	packet.type = SC_GAME_BREAKPLATFORM;
	packet.objectID = objectID;
	return packet;
}

void MainServer::SendAllBroadCast(void* buf, const int bufSize)
{
	for (auto& other : mObjects) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			SendPacketWithID(OtherPlayer->GetSocketID(), buf, bufSize);
			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}

	}
}




void MainServer::SendRoomBroadCast(const int roomID, void* buf, const int bufSize)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			SendPacketWithID(OtherPlayer->GetSocketID(), buf, bufSize);
			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}

	}
}


void MainServer::SendRoomSomeoneExcept(const int roomID, const int exceptSocketID, void* buf, const int bufSize)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == exceptSocketID)continue; // 제외해줄 ID
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			SendPacketWithID(OtherPlayer->GetSocketID(), buf, bufSize);
			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}
	}
}

void MainServer::SendMySelf(const int receiverSocketID, void* buf, const int bufSize)
{

	auto player = dynamic_cast<Player*>(mObjects[receiverSocketID]);
	if (nullptr != player)
		player->SendPacket(buf, bufSize);

}

template<class T>
inline void MainServer::SendRoomOthersToMe(const int roomID, const int receiverSocketID, const int exceptSocketID, T(MainServer::* fp)(const int))
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == exceptSocketID)continue; // 제외해줄 ID
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			T packet = (this->*fp)(OtherPlayer->GetSocketID());
			SendPacketWithID(receiverSocketID, (void*)(&packet), sizeof(T));
			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}
	}
}

void MainServer::connectLobbyServer()
{
	mLobbyServer = new LobbyServer();
	dynamic_cast<LobbyServer*>(mLobbyServer)->init();
}

bool MainServer::setPlayerInRoom(Player* player, const char verification[MAX_NAME_SIZE])
{
	for (auto& tRoom : mRooms)
	{
		auto& room = tRoom.second;
		if (room->IsRoomReadyComplete())
		{
			int result = room->GetPlayerRoomSyncID(player->GetUID());
			if (0 <= result)
			{
				sPlayerInfo pInfo = room->GetPlayerInfo(player->GetUID());
				if (-1 != pInfo.UID)
				{
					if (0 != strcmp(pInfo.hashs, verification))
					{
						DEBUGMSGONEPARAM("검증값이 다름.(부정접속) [%d]번째플레이어\n", player->GetSocketID());
						DEBUGMSGONEPARAM("검증값이 다름.(부정접속) [%s]플레이어\n", player->GetID());
						return false;
					}
					if (pInfo.RoomID != tRoom.first)
					{
						DEBUGMSGONEPARAM("플레이어가 로비서버에게 받은 방이 아닌 다른곳에 존재함.(부정접속) [%d]번째플레이어\n", player->GetSocketID());
						return false;
					}
					player->SetRoomSyncID(result);
					player->SetRoomID(tRoom.first);
					player->SetDepartment(pInfo.Department);
					player->SetID(pInfo.ID);
					player->SetNickName(pInfo.NickName);
					//한명이라도 들어오면 일단 타이머 세팅
					room->setGameStartTimerStartOnce();
					return true;
				}
				else {
					DEBUGMSGONEPARAM("플레이어정보가 방 매칭정보에 존재하지않음. [%d]번째플레이어\n", player->GetSocketID());
					return false;
				}
			}
			else {
				DEBUGMSGONEPARAM("플레이어가 방 매칭정보에 존재하지않음. [%d]번째플레이어\n", player->GetSocketID());
				return false;
			}
		}
	}
	DEBUGMSGONEPARAM("플레이어와 방 매칭 도중 버그가 발생함[%d]번째플레이어\n", player->GetUID());
	return false;
}

bool MainServer::initRoom(const std::string& mapName)
{
	std::ifstream is{ mapName };
	if (!is.is_open())
	{
		DEBUGMSGONEPARAM("[%s]읽기 실패\n", mapName.c_str());
	}

	return false;
}




void MainServer::ProcessPacket(const int client_id, unsigned char* p)
{
	unsigned char packet_type = p[1];
	Object* object = mObjects[client_id];

	switch (packet_type) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* packet = reinterpret_cast<CS_LOGIN_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(mObjects[client_id]);
		//character->skintype = packet->skintype;
		player->SetSocketID(client_id);
		DEBUGMSGONEPARAM("플레이어%d 접속\n", client_id);
		if (packet->roomID > MAX_ROOM)
		{
			//에러
			DEBUGMSGNOPARAM("잘못된 방에 접속하려 시도함.\n");
			break;
		}
		//로비랑 연결 안 됐을때는 아래 setPlayerInRoom is ignored, 여기서 세팅
		//에디터에서 개발 편하게 하려고 넣은 코드
		//player->SetRoomID(0);

		//uid랑 name이랑 비교구분해서 검증작업 하기  
		player->SetID(packet->name);
		player->SetUID(packet->uID);
		// 단순 검증 말고, hash를 이용한 검증이라던가 가 필요함.
		// 너무 보안적으로 취약함.. 뭘믿고 클라에서 보내는 데이터를 신용하지?
		// 로비서버가 암호화된 키를 게임서버에게 줌. 저장.
		// 로비서버가 클라한테 겜서버로 가라고 할 때 이 암호화된것을 줌.
		// 클라가 암호화된 무언가를 겜서버로 담아 보냄.
		// 겜서버는 로비서버가 준 암호화랑 클라가 보낸 암호화랑 비교함 다르면 부정접속.
		// UID를 먼저 받고, uid로 playerInfo에서 암호화된 값과, packet의 암호화된 값을 비교하면 그나마 좀 안정적일것임.
		// 이유: 해킹범이 uid를 변조했을 때, 이 암호화 된 무언가의 랜덤값 또한 맞춰야함. 그런데 진짜 로비서버에서 랜덤으로 생성해서 주면 어떻게 맞출건데? 

		//
		if (false == setPlayerInRoom(player, packet->hashs))
		{
			/*
				나중에 여기에 제대로 된 방 id가 안나오면 접속을 끊어버려야함. 부정접속
			*/
			DEBUGMSGONEPARAM("[%d]플레이어 부정접속. 접속 해제", player->GetSocketID());
			player->DisConnectAndReset();

			break;
		}


		Room* pRoom = mRooms[player->GetRoomID()];
		pRoom->AddObject(player);
		{
			DEBUGMSGONEPARAM("[%d]\n", player->GetRoomSyncID());
			SC_LOGIN_OK_PACKET sPacket = make_login_ok_packet(player->GetSocketID(),player->GetRoomSyncID(), "none");
			SendMySelf(client_id, (void*)&sPacket, sizeof(sPacket));
		}
		{
			SC_PING_PACKET sPacket = make_ping_packet();
			SendMySelf(client_id, (void*)&sPacket, sizeof(sPacket));
		}


		player->GetStateLockRef().lock();
		player->SetSocketState(eSocketState::ST_INGAME);
		player->GetStateLockRef().unlock();

		//나를 상대에게
		{
			auto spacket = make_player_add_packet(player->GetSocketID());
			SendRoomSomeoneExcept(player->GetRoomID(), player->GetSocketID(), (void*)&spacket, sizeof(spacket));
		}


		//방안의 모든 플레이어 정보를 나에게로
		SendRoomOthersToMe(player->GetRoomID(), player->GetSocketID(), player->GetSocketID(), &MainServer::make_player_add_packet);


		DEBUGMSGNOPARAM("player load ok Send.\n");
		auto loadpacket = make_game_playerload_ok_packet();
		SendMySelf(player->GetSocketID(), (void*)&loadpacket, sizeof(loadpacket));

		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* packet = reinterpret_cast<CS_MOVE_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr)
		{
			DEBUGMSGNOPARAM("player is nullptr.\n");
			break;
		}
		Room* pRoom = mRooms[player->GetRoomID()];


		player->SetMoveTime(packet->move_time);
		player->SetPosition(Vector3f(packet->x, packet->y, packet->z));
		player->SetRotate(Vector4f(packet->rx, packet->ry, packet->rz, packet->rw));

		{
			auto sPacket = make_move_packet(player->GetSocketID(), packet->inair, packet->speed, packet->sx, packet->sy, packet->sz);
			SendRoomBroadCast(player->GetRoomID(), (void*)&sPacket, sizeof(sPacket));
		}
		break;
	}
	case CS_GOAL: {
		CS_GOAL_PACKET* packet = reinterpret_cast<CS_GOAL_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr)
		{
			DEBUGMSGNOPARAM("player is nullptr.\n");
			break;
		}
		Room* pRoom = mRooms[player->GetRoomID()];
		

		DEBUGMSGONEPARAM("player Num[%d] Arrive Overlapped Packet \n", player->GetSocketID());
		if (false == player->CanPlayerArrive()) break;	//한 번 도착했으면 패킷 무시

		pRoom->GetRoomStateLockRef().lock();
		if (pRoom->GetRoomState() != eRoomState::ST_INGAME)
		{
			pRoom->GetRoomStateLockRef().unlock();
			break;			
		}
		pRoom->PlayerArrive(player);
		pRoom->GetRoomStateLockRef().unlock();

		DEBUGMSGONEPARAM("player Num[%d] Arrive Complete\n", player->GetSocketID());

		{
			auto sPacket = make_player_arrive_packet(player->GetRoomSyncID());
			SendRoomSomeoneExcept(player->GetRoomID(), player->GetSocketID(), (void*)&sPacket, sizeof(sPacket));
		}
		send_player_result_packet(player->GetUID(), player->GetRank(), player->GetRoomID(), false);

		break;
	}
	case CS_PING: {
		CS_PING_PACKET* packet = reinterpret_cast<CS_PING_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr)
		{
			DEBUGMSGNOPARAM("player is nullptr.\n");
			break;
		}
		auto ping = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count() - packet->ping;
		player->SetPing(ping);

		break;
	}
	case CS_ACTION:
	{
		CS_ACTION_PACKET* packet = reinterpret_cast<CS_ACTION_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr)
		{
			DEBUGMSGNOPARAM("player is nullptr.\n");
			break;
		}

		{
			auto sPacket = make_action_packet(player->GetRoomSyncID(), packet->action);
			SendRoomSomeoneExcept(player->GetRoomID(), player->GetSocketID(), (void*)&sPacket, sizeof(sPacket));
		}

		break;
	}
	case CS_GAME_PLAYERLOAD_ACK:
	{
		CS_GAME_PLAYERLOAD_ACK_PACKET* packet = reinterpret_cast<CS_GAME_PLAYERLOAD_ACK_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr)
		{
			DEBUGMSGNOPARAM("player is nullptr.\n");
			break;
		}
		Room* pRoom = mRooms[player->GetRoomID()];
		DEBUGMSGNOPARAM("TheGameIsWaitting\n");
		cout << player->GetID() << endl;
		pRoom->PlayerCntIncrease();
		//--
		//if (pRoom->IsAllPlayerReady())
		//{
		//	DEBUGMSGNOPARAM("TheGameIsWaitting Packet Come In \n");
		//	SC_GAME_WAITTING_PACKET spacket = make_game_watting_packet();
		//	SendRoomBroadCast(player->GetRoomID(), (void*)&spacket, sizeof(spacket));
		//	TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_START, eEventType::TYPE_BROADCAST_ROOM, 4000, NULL, player->GetRoomID());
		//}
		//--
		break;
	}
	case CS_GAME_BREAKDOOR:
	{
		CS_GAME_BREAKDOOR_PACKET* packet = reinterpret_cast<CS_GAME_BREAKDOOR_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr)
		{
			DEBUGMSGNOPARAM("player is nullptr.\n");
			break;
		}

		auto sPacket = make_game_breakdoor_packet(packet->objectID);
		SendRoomBroadCast(player->GetRoomID(), (void*)(&sPacket), sizeof(sPacket));

		break;
	}
	case CS_GAME_BREAKPLATFORM:
	{
		CS_GAME_BREAKPLATFORM_PACKET* packet = reinterpret_cast<CS_GAME_BREAKPLATFORM_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr)
		{
			DEBUGMSGNOPARAM("player is nullptr.\n");
			break;
		}
		TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_BREAKPLATFORM, eEventType::TYPE_BROADCAST_ROOM, DELAY_TIME_EXEC_BREAKPLATFORM, packet->objectID, player->GetRoomID());

		break;
	}
	default:
		break;
	}
}

void MainServer::ProcessPacketLobby(const int serverID, unsigned char* p)
{
	unsigned char packet_type = p[1];
	Server* object = mLobbyServer;

	switch (packet_type) {
	case LG_USER_INTO_GAME: {
		LG_USER_INTO_GAME_PACKET* packet = reinterpret_cast<LG_USER_INTO_GAME_PACKET*>(p);
		Room* activeRoom = mRooms[packet->roomID];
		mRooms[packet->roomID]->ActiveRoom();
		sPlayerInfo playerinfo{ packet->id,packet->name,static_cast<eDepartment>(packet->department),static_cast<eEquipmentFlags>(packet->equipmentflag),packet->roomID };
		playerinfo.UID = packet->uID;
		strcpy_s(playerinfo.hashs, packet->hashs);
		if (true == activeRoom->SettingRoomPlayer(playerinfo, packet->roomMax))
		{
			DEBUGMSGONEPARAM("%d번째 방 활성화 완료.", packet->roomID);
			DEBUGMSGONEPARAM("인원수 [%d]\n", packet->roomMax);
			send_room_ready_packet(packet->roomID);
			TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_PING, eEventType::TYPE_BROADCAST_ROOM, 3000, NULL, packet->roomID);

		}

		break;
	}
	default:
		break;
	}
}
