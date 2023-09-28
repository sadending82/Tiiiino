#include "MainServer.h"

#include "../../Object/Player/Player.h"
#include "../../Thread/WorkerThread/WorkerThread.h"
#include "../../Room/Room.h"
#include "../LobbyServer/LobbyServer.h"
#include "../../../../../ServerProtocol.h"

MainServer* gMainServer;

using namespace std;

MainServer::MainServer()
{

}

MainServer::~MainServer()
{
	for (auto& object : mObjects) {
		if (dynamic_cast<Player*>(object) == nullptr) break;

		auto player = reinterpret_cast<Player*>(object);
		player->DisConnect();
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
		Room* room = new Room();
		room->Init();
		mRooms.insert(make_pair(i, room));
	}
	connectLobbyServer();
	mWorkerThreadRef = new WorkerThread(this);

	std::cout << "Creating Worker Threads\n";
	//vector<thread> worker_threads;
	//thread timer_thread{ TimerThread };
	//thread logger_thread{ LogThread };

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	for (int i = 0; i < (int)si.dwNumberOfProcessors; ++i)
		mWorkerThreads.emplace_back(&WorkerThread::doThread, mWorkerThreadRef);
}

void MainServer::run()
{
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
		player->SendPacket(&buf, sizeof(bufSize));
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

SC_LOGIN_OK_PACKET MainServer::make_login_ok_packet(const int player_id,const char* playername)
{
	auto player = dynamic_cast<Player*>(mObjects[player_id]);
	SC_LOGIN_OK_PACKET packet;
	memset(&packet, 0, sizeof(SC_LOGIN_OK_PACKET));

	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;

	packet.id = player_id;
	//wcscpy(packet.name, playername);
	return packet;
}

void MainServer::send_player_add_packet(const int playerID, void* buf, const int bufSize)
{
	auto player = dynamic_cast<Player*>(mObjects[playerID]);
	player->SendPacket(&buf, sizeof(bufSize));

}

SC_ADD_PLAYER_PACKET MainServer::make_player_add_packet(const int playerID)
{
	Player* player = dynamic_cast<Player*>(mObjects[playerID]);
	SC_ADD_PLAYER_PACKET sendpacket;
	sendpacket.id = playerID;
	//strcpy_s(sendpacket.name, character->name);
	sendpacket.size = sizeof(sendpacket);
	sendpacket.type = SC_ADD_PLAYER;

	sendpacket.x = player->GetPosition().x;;
	sendpacket.y = player->GetPosition().y;
	sendpacket.z = player->GetPosition().z;
	sendpacket.rx = player->GetRotate().x;
	sendpacket.ry = player->GetRotate().y;
	sendpacket.rz = player->GetRotate().z;
	sendpacket.rw = player->GetRotate().w;
	return sendpacket;
}

void MainServer::send_room_ready_packet(const int roomID)
{
	GL_ROOM_READY_PACKET packet;
	memset(&packet, 0, sizeof(GL_ROOM_READY_PACKET));

	packet.size = sizeof(packet);
	packet.type = GL_ROOM_READY;
	packet.roomID = roomID;

	mLobbyServer->SendPacket(&packet, sizeof(packet));
}


void MainServer::send_move_packet(const int player_id,const int mover_id, const bool inair,const float value, const float sx, const float sy, const float sz)
{
	auto player = reinterpret_cast<Player*>(mObjects[player_id]);
	SC_MOVE_PLAYER_PACKET packet{};
	packet.id = mover_id;
	packet.size = sizeof(packet);
	packet.type = SC_MOVE_PLAYER;
	//packet.inair = inair;
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

SC_MOVE_PLAYER_PACKET MainServer::make_move_packet(const int moverID, const bool inair, const float value, const float sx, const float sy, const float sz)
{
	SC_MOVE_PLAYER_PACKET packet{};
	packet.id = moverID;
	packet.size = sizeof(packet);
	packet.type = SC_MOVE_PLAYER;
	//packet.inair = inair;
	packet.x = mObjects[moverID]->GetPosition().x;
	packet.y = mObjects[moverID]->GetPosition().y;
	packet.z = mObjects[moverID]->GetPosition().z;
	packet.rx = mObjects[moverID]->GetRotate().x;
	packet.ry = mObjects[moverID]->GetRotate().y;
	packet.rz = mObjects[moverID]->GetRotate().z;
	packet.rw = mObjects[moverID]->GetRotate().w;
	auto mover = dynamic_cast<Player*>(mObjects[moverID]);
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

void MainServer::SendAllBroadCast(void(MainServer::* fp)(int), int value1)
{
}



void MainServer::SendAllBroadCast(void(MainServer::* fp)(int, int), int value1, int value2)
{
	(this->*fp)(value1, value2);
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

void MainServer::SendRoomBroadCast(const int roomID, void(MainServer::* fp)(int), int playerID)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		// && OtherPlayer->GetSocketID() == client_id movepacket에서핑테스트를 하려면 나도 나한테 보내야함 처리는 클라에서
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			(this->*fp)(playerID);

			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}

	}
}

void MainServer::SendRoomBroadCast(const int roomID, void(MainServer::* fp)(const int, void*), const int playerID, void* buf)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		// && OtherPlayer->GetSocketID() == client_id movepacket에서핑테스트를 하려면 나도 나한테 보내야함 처리는 클라에서
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			(this->*fp)(playerID, buf);

			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}

	}
}

void MainServer::SendRoomBroadCast(const int roomID, void(MainServer::* fp)(int, int), int playerID, int value2)
{
}

void MainServer::SendRoomBroadCast(const int roomID, void(MainServer::* fp)(const int, const int, const bool, const float, const float, const float, const float), 
	const int playerID, const int value2, const bool value3, const float value4, const float value5, const float value6, const float value7)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		// && OtherPlayer->GetSocketID() == client_id movepacket에서핑테스트를 하려면 나도 나한테 보내야함 처리는 클라에서
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			(this->*fp)(playerID, value2, value3, value4, value5, value6, value7);

			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}
	}
}


void MainServer::SendRoomSomeoneExcept(const int roomID, const int exceptID, void* buf, const int bufSize)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == exceptID)continue; // 제외해줄 ID
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

void MainServer::SendRoomSomeoneExcept(const int roomID, const int exceptID, void(MainServer::* fp)(const int))
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == exceptID)continue; // 제외해줄 ID
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			(this->*fp)(OtherPlayer->GetSocketID());

			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}
	}
}

void MainServer::SendRoomSomeoneExcept(const int roomID, const int exceptID, void(MainServer::* fp)(const int, void*), void* buf,const int bufSize)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == exceptID)continue; // 제외해줄 ID
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			SendPacket(buf, bufSize);
			//(this->*fp)(OtherPlayer->GetSocketID(), buf);
			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}
	}
}

void MainServer::SendRoomSomeoneExcept(const int roomID, const int exceptID, void(MainServer::* fp)(const int, void*, const int), void* buf, const int bufSize)
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == exceptID)continue; // 제외해줄 ID
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			SendPacketWithID(OtherPlayer->GetSocketID(), buf, bufSize);
			(this->*fp)(OtherPlayer->GetSocketID(), buf,bufSize);
			continue;
		}
		else {
			OtherPlayer->GetStateLockRef().unlock();
		}
	}
}

void MainServer::SendMySelf(const int receiverID, void* buf, const int bufSize)
{

	auto player = dynamic_cast<Player*>(mObjects[receiverID]);
	if (nullptr != player)
		player->SendPacket(buf, bufSize);

}

template<class T>
inline void MainServer::SendRoomOthersToMe(const int roomID, const int receiveID, const int exceptID, T(MainServer::* fp)(const int))
{
	Room* pRoom = mRooms[roomID];
	for (auto& other : pRoom->GetObjectsRef()) {
		Player* OtherPlayer = dynamic_cast<Player*>(other);
		if (OtherPlayer == nullptr) break;
		if (OtherPlayer->GetSocketID() == exceptID)continue; // 제외해줄 ID
		if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		OtherPlayer->GetStateLockRef().lock();
		if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
			OtherPlayer->GetStateLockRef().unlock();
			T packet = (this->*fp)(OtherPlayer->GetSocketID());
			SendPacketWithID(receiveID, (void*)(&packet), sizeof(T));
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
		cout << " 플레이어 " << client_id << "접속\n";
		if (packet->roomID > MAX_ROOM)
		{
			//에러
			cout << "잘못된 방에 입장하려 시도함.\n";
			break;
		}
		//로비랑 연결 안 됐을때는 아래 for문이 무시되므로 여기서 세팅
		//에디터에서 개발 편하게 하려고 넣은 코드
		player->SetRoomID(0);


		for (auto tRoom : mRooms)
		{
			auto& room = tRoom.second;
			if (room->IsRoomActive())
			{
				if (room->FindPlayerInfo(packet->uID, packet->name))
				{
					player->SetRoomID(tRoom.first);
				}
			}
		}

		/*
			나중에 여기에 제대로 된 방 id가 안나오면 접속을 끊어버려야함. 부정접속
		*/

		Room* pRoom = mRooms[player->GetRoomID()];
		pRoom->AddObject(player);


		//{
		//	DEBUGMSGNOPARAM("여기 제대로도착");
		//	SC_LOGIN_OK_PACKET sendpacket = make_login_ok_packet(client_id, "none");
		//	SendMySelf(client_id, (void*)&sendpacket, sizeof(sendpacket));
		//}
		send_login_ok_packet(client_id, "none");


		//나를 상대에게


		//auto spacket = make_player_add_packet(client_id);
		//SendRoomSomeoneExcept(player->GetRoomID(), client_id, (void*)&spacket, sizeof(spacket));
		for (auto& other : pRoom->GetObjectsRef()) {
			Player* OtherPlayer = dynamic_cast<Player*>(other);
			if (OtherPlayer == nullptr) break;
			if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID || OtherPlayer->GetSocketID() == client_id) continue;
		
		
			OtherPlayer->GetStateLockRef().lock();
			if (eSocketState::ST_INGAME != OtherPlayer->GetSocketState()) {
				OtherPlayer->GetStateLockRef().unlock();
				continue;
			}
			OtherPlayer->GetStateLockRef().unlock();
		
			SC_ADD_PLAYER_PACKET sendpacket;
			sendpacket.id = client_id;
			//strcpy_s(sendpacket.name, character->name);
			sendpacket.size = sizeof(sendpacket);
			sendpacket.type = SC_ADD_PLAYER;
		
			sendpacket.x = player->GetPosition().x;;
			sendpacket.y = player->GetPosition().y;
			sendpacket.z = player->GetPosition().z;
			sendpacket.rx = player->GetRotate().x;
			sendpacket.ry = player->GetRotate().y;
			sendpacket.rz = player->GetRotate().z;
			sendpacket.rw = player->GetRotate().w;
			OtherPlayer->SendPacket(&sendpacket, sizeof(sendpacket));
		}

		//방안의 모든 플레이어 정보를 나에게로
		//SendRoomOthersToMe(player->GetRoomID(), player->GetSocketID(), player->GetSocketID(), &MainServer::make_player_add_packet);

		for (auto& other : pRoom->GetObjectsRef()) {
			Player* OtherPlayer = dynamic_cast<Player*>(other);
			if (OtherPlayer == nullptr) break;
			if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID || OtherPlayer->GetSocketID() == client_id) continue;
		
			OtherPlayer->GetStateLockRef().lock();
			if (eSocketState::ST_INGAME != OtherPlayer->GetSocketState()) {
				OtherPlayer->GetStateLockRef().unlock();
				continue;
			}
			OtherPlayer->GetStateLockRef().unlock();
		
			SC_ADD_PLAYER_PACKET sendpacket;
			sendpacket.id = OtherPlayer->GetSocketID();
			//strcpy_s(sendpacket.name, OtherPlayer->name);
			sendpacket.size = sizeof(sendpacket);
			sendpacket.type = SC_ADD_PLAYER;
			//어차피 서버에서 다시 판결하기때문에 여기 넣어봤자 쓸모 없음.
			player->SendPacket(&sendpacket, sizeof(sendpacket));
		}

		player->GetStateLockRef().lock();
		player->SetSocketState(eSocketState::ST_INGAME);
		player->GetStateLockRef().unlock();
		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* packet = reinterpret_cast<CS_MOVE_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr) break;
		Room* pRoom = mRooms[player->GetRoomID()];

		player->SetMoveTime(packet->move_time);
		player->SetPosition(Vector3f(packet->x, packet->y, packet->z));
		player->SetRotate(Vector4f(packet->rx, packet->ry, packet->rz, packet->rw));

		//{
		//	auto sPacket = make_move_packet(player->GetSocketID(), packet->inair, packet->speed, packet->sx, packet->sy, packet->sz);
		//	SendRoomBroadCast(player->GetRoomID(), (void*)&sPacket, sizeof(sPacket));
		//}
		for (auto& other : pRoom->GetObjectsRef()) {
			Player* OtherPlayer = dynamic_cast<Player*>(other);
			if (OtherPlayer == nullptr) break;
			// && OtherPlayer->GetSocketID() == client_id movepacket에서핑테스트를 하려면 나도 나한테 보내야함 처리는 클라에서
			if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID || OtherPlayer->GetSocketID() == client_id) continue;
			if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		
		
			OtherPlayer->GetStateLockRef().lock();
			if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
				OtherPlayer->GetStateLockRef().unlock();
				send_move_packet(OtherPlayer->GetSocketID(), client_id, packet->inair, packet->speed, packet->sx, packet->sy, packet->sz);
		
				continue;
			}
			else {
				OtherPlayer->GetStateLockRef().unlock();
			}
		
		}
		break;
	}
	case CS_GOAL: {
		CS_GOAL_PACKET* packet = reinterpret_cast<CS_GOAL_PACKET*>(p);
		Player* player = dynamic_cast<Player*>(object);
		if (player == nullptr) break;

		DEBUGMSGONEPARAM("player Num[%d] Arrive Overlapped Packet \n", player->GetSocketID());

		if (false == player->CanPlayerArrive()) break;	//한 번 도착했으면 패킷 무시

		Room* pRoom = mRooms[player->GetRoomID()];

		pRoom->PlayerArrive(player);
		DEBUGMSGONEPARAM("player Num[%d] Arrive Complete\n", player->GetSocketID());

		{
			auto sPacket = make_player_arrive_packet(player->GetSocketID());
			SendRoomSomeoneExcept(player->GetRoomID(), player->GetSocketID(), (void*)&sPacket, sizeof(sPacket));
		}
		//for (auto& other : pRoom->GetObjectsRef()) {
		//	Player* OtherPlayer = dynamic_cast<Player*>(other);
		//	if (OtherPlayer == nullptr) break;
		//	if (OtherPlayer->GetSocketID() == client_id) continue;	//내가 도착한걸 나한테 보낼 필요는 없음.
		//	if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID) continue;
		//
		//
		//	OtherPlayer->GetStateLockRef().lock();
		//	if (eSocketState::ST_INGAME == OtherPlayer->GetSocketState()) {
		//		OtherPlayer->GetStateLockRef().unlock();
		//		send_player_arrive_packet(OtherPlayer->GetSocketID(), client_id);
		//
		//		continue;
		//	}
		//	else {
		//		OtherPlayer->GetStateLockRef().unlock();
		//	}
		//
		//}
		break;
	}
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

		if (true == activeRoom->SettingRoomPlayer(packet->uID, packet->name, packet->roomMax))
		{
#ifdef _DEBUG
			cout << packet->roomID << "번째 방 활성화 완료.\n";
#endif
			send_room_ready_packet(packet->roomID);
		}

		break;
	}
	case CS_MOVE: {

		break;
	}
	}
}
