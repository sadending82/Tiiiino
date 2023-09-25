#include "MainServer.h"

#include "../../Object/Player/Player.h"
#include "../../Thread/WorkerThread/WorkerThread.h"
#include "../../Room/Room.h"
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
		mRooms.insert(make_pair(i,room));
	}
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

void MainServer::send_room_ready_packet(const int roomID)
{
	GL_ROOM_READY_PACKET packet;
	memset(&packet, 0, sizeof(GL_ROOM_READY_PACKET));

	packet.size = sizeof(packet);
	packet.type = GL_ROOM_READY;
	packet.roomID = roomID;

	mLobbyServer->SendPacket(&packet, sizeof(packet));
}


void MainServer::send_move_packet(int player_id, int mover_id, const bool& inair, float value, const float& sx, const float& sy, const float& sz)
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
	if(mover)
		packet.move_time = mover->GetMoveTime();
	packet.speed = value;
	packet.sx = sx;
	packet.sy = sy;
	packet.sz = sz;
	player->SendPacket(&packet, sizeof(packet));
}

void MainServer::ConnectLobbyServer()
{

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
		if (packet->roomID > MAX_ROOM)
		{
			//에러
			cout << "잘못된 방에 입장하려 시도함.\n";
			break;
		}
		player->SetRoomID(packet->roomID);
		Room* pRoom = mRooms[player->GetRoomID()];
		pRoom->AddObject(player);

		send_login_ok_packet(client_id, "none");


		//나를 상대에게
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

		//상대를 나에게
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
		for (auto& other : pRoom->GetObjectsRef()) {
			Player* OtherPlayer = dynamic_cast<Player*>(other);
			if (OtherPlayer == nullptr) break;
			// && OtherPlayer->GetSocketID() == client_id movepacket에서핑테스트를 하려면 나도 나한테 보내야함 처리는 클라에서
			if (OtherPlayer->GetSocketID() == INVALID_SOCKET_ID ) continue;


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

		if (true == activeRoom->SettingRoomPlayer(packet->name, packet->passWord, packet->roomMax))
		{
			send_room_ready_packet(packet->roomID);
		}

		break;
	}
	case CS_MOVE: {

		break;
	}
	}
}
