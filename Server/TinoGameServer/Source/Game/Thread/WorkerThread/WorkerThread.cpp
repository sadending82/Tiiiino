#include "WorkerThread.h"
#include "../TimerThread/TimerThread.h"
#include "../../Object/Player/Player.h"
#include "../../Object/MapObject/MoveObject/M_Door/M_Door.h"
#include "../../Server/MainServer/MainServer.h"
#include "../../Network/Network.h"
#include "../../Server/LobbyServer/LobbyServer.h"
#include "../../Room/Room.h"

WorkerThread::WorkerThread(MainServer* Server)
	:mMainServer(Server)
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::doThread()
{

	for (;;) {
		DWORD bytes;
		LONG64 iocp_key;
		WSAOVERLAPPED* overlapped;
		BOOL ret = GetQueuedCompletionStatus(mMainServer->GetIOCPHandle(), &bytes, (PULONG_PTR)&iocp_key, &overlapped, INFINITE);
		int client_id = static_cast<int>(iocp_key);
		WSA_OVER_EX* wsa_ex = reinterpret_cast<WSA_OVER_EX*>(overlapped);
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			DEBUGMSGNOPARAM("GQCS Error\n");
			mMainServer->ErrorDisplay(err_no);
			if (wsa_ex->GetCmd() == eCOMMAND_IOCP::CMD_SEND)
				delete wsa_ex;
			continue;
		}

		switch (wsa_ex->GetCmd()) {
		case eCOMMAND_IOCP::CMD_RECV: {
			if (bytes == 0) {
				auto t = dynamic_cast<Player*>(mMainServer->GetObjects()[client_id]);
				if (t) t->DisConnect();
				break;
			}

			Player* player = dynamic_cast<Player*>(mMainServer->GetObjects()[client_id]);
			int To_Process_Bytes = bytes + player->GetPrevSize();
			unsigned char* packets = wsa_ex->GetBuf();	//wsa_ex == player->wsa_ex.buf

			if (To_Process_Bytes >= packets[0])
			{
				do {
					mMainServer->ProcessPacket(client_id, packets);
					To_Process_Bytes -= packets[0];
					packets += packets[0];
				} while ((To_Process_Bytes & ~0) && (To_Process_Bytes >= packets[0]));
			}
			//To_Process_Bytes != 0 이면 packets보다 큰지 확인해야 알 수 있지만
			//to_Process_Bytes가 packets보다 크면, To_Process_Bytes != 0인건 확실하므로, 이때는 필요없는 구문
			//뒷구문이 앞으로오는건 의미가 없음. 앞구문에서 성공하여 cmp를 한 번 만으로 끝낼 수 있는 기대효과
			player->PreRecvPacket(packets, To_Process_Bytes);
			player->RecvPacket();
			break;
		}
		case eCOMMAND_IOCP::CMD_SERVER_RECV:
		{
			if (bytes == 0) {
				//auto t = dynamic_cast<Player*>(mMainServer->GetObjects()[client_id]);
				//if (t) t->DisConnect();
				std::cout << "버그버그버그 서버 연결 버그 \n";
				//Disconnect(client_id);
				break;
			}

			LobbyServer* lobbyServer = dynamic_cast<LobbyServer*>(mMainServer->GetLobbyServer());
			int To_Process_Bytes = bytes + lobbyServer->GetPrevSize();
			unsigned char* packets = wsa_ex->GetBuf();	//wsa_ex == player->wsa_ex.buf

			if (To_Process_Bytes >= packets[0])
			{
				do {
					mMainServer->ProcessPacketLobby(client_id, packets);
					To_Process_Bytes -= packets[0];
					packets += packets[0];
				} while ((To_Process_Bytes & ~0) && (To_Process_Bytes >= packets[0]));
			}
			//To_Process_Bytes != 0 이면 packets보다 큰지 확인해야 알 수 있지만
			//to_Process_Bytes가 packets보다 크면, To_Process_Bytes != 0인건 확실하므로, 이때는 필요없는 구문
			//뒷구문이 앞으로오는건 의미가 없음. 앞구문에서 성공하여 cmp를 한 번 만으로 끝낼 수 있는 기대효과
			lobbyServer->PreRecvPacket(packets, To_Process_Bytes);
			lobbyServer->RecvPacket();
			break;
		}
		case eCOMMAND_IOCP::CMD_ACCEPT: {
			std::cout << "Accept Completed.\n";
			SOCKET c_socket = *(reinterpret_cast<SOCKET*>(wsa_ex->GetBuf()));
			LINGER  ling = { 0, };
			ling.l_onoff = 1;   // LINGER 옵션 사용 여부  
			ling.l_linger = 0;  // LINGER Timeout 설정  
			// LINGER 옵션을 Socket에 적용  
			setsockopt(c_socket, SOL_SOCKET, SO_LINGER, (CHAR*)&ling, sizeof(ling));

			int new_id = mMainServer->GenerateID();
			if (new_id != -1)
			{
				Player* player = reinterpret_cast<Player*>(mMainServer->GetObjects()[new_id]);
				player->SetSocketID(new_id);
				player->AcceptSetting(eSocketState::ST_ACCEPT, eCOMMAND_IOCP::CMD_RECV, c_socket);

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), mMainServer->GetIOCPHandle(), new_id, 0);
				player->PreRecvPacket(NULL, 0);
				player->RecvPacket();

			}
			else if (new_id == -1)
			{
				closesocket(c_socket);
			}

			ZeroMemory(&wsa_ex->GetWsaOver(), sizeof(wsa_ex->GetWsaOver()));
			c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
			*(reinterpret_cast<SOCKET*>(wsa_ex->GetBuf())) = c_socket;
			AcceptEx(mMainServer->GetSocket(), c_socket, wsa_ex->GetBuf() + 8, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &wsa_ex->GetWsaOver());
			break;
		}
		case eCOMMAND_IOCP::CMD_GAME_START:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			DEBUGMSGNOPARAM("한번만 오는지 GAME END\n");
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			{
				auto sPacket = mMainServer->make_game_start_packet();
				mMainServer->SendRoomBroadCast(roomID, (void*)&sPacket, sizeof(sPacket));
			}
			break;
		}
		case eCOMMAND_IOCP::CMD_GAME_END:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			DEBUGMSGNOPARAM("한번만 오는지 GAME END\n");
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			{
				auto sPacket = mMainServer->make_game_end_packet();	//판정은 클라가 알아서.
				mMainServer->SendRoomBroadCast(roomID, (void*)&sPacket, sizeof(sPacket));
			}
			TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_RESET, eEventType::TYPE_TARGET, 10000, 0, roomID);
			break;
		}
		case eCOMMAND_IOCP::CMD_GAME_RESET:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			mMainServer->GetRooms()[roomID]->ResetGameRoom();
			mMainServer->send_room_reset_packet(roomID);

			break;
		}
		case eCOMMAND_IOCP::CMD_GAME_COUNTDOWN_START:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			matchTimerType(eventType);
			{
				auto sPacket = mMainServer->make_game_countdown_start_packet();
				mMainServer->SendRoomBroadCast(roomID, (void*)&sPacket, sizeof(sPacket));
			}
			TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_END, eEventType::TYPE_BROADCAST_ROOM, 10000, NULL, roomID);

			break;
		}
		case eCOMMAND_IOCP::CMD_PING:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			{
				auto sPacket = mMainServer->make_ping_packet();
				mMainServer->SendRoomBroadCast(roomID, (void*)&sPacket, sizeof(sPacket));
				TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_PING, eEventType::TYPE_BROADCAST_ROOM, 1000, NULL, roomID);
			}
			break;
		}
		case eCOMMAND_IOCP::CMD_GAME_DOORSYNC:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			int roomSyncID = client_id;
			M_Door* obj = dynamic_cast<M_Door*>(mMainServer->GetRooms()[roomID]->GetObjectsRef()[roomSyncID]);
			if (!obj)
			{
				DEBUGMSGONEPARAM("M_Door Doensn't exit [%d]\n", roomSyncID);
				break;
			}
			for (Object*& Object : mMainServer->GetRooms()[roomID]->GetObjectsRef())
			{
				Player* player = dynamic_cast<Player*>(Object);
				if (!player) break;
				auto sPacket = mMainServer->make_game_doorsync_packet(obj->MeasureSyncTime(player->GetPing()), roomSyncID);
				mMainServer->SendMySelf(player->GetSocketID(), (void*)&sPacket, sizeof(sPacket));
			}
			obj->Setting();
			TimerThread::MakeTimerEventMilliSec(eCOMMAND_IOCP::CMD_GAME_DOORSYNC, eEventType::TYPE_BROADCAST_ROOM, obj->GetWaitMilliTime(), roomSyncID, roomID);
			break;
		}
		case eCOMMAND_IOCP::CMD_GAME_BREAKDOOR:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			DEBUGMSGNOPARAM("break door\n");
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			int objectID = client_id;
			auto sPacket = mMainServer->make_game_breakdoor_packet(objectID);
			mMainServer->SendRoomBroadCast(roomID, (void*)&sPacket, sizeof(sPacket));
			break;
		}
		case eCOMMAND_IOCP::CMD_GAME_BREAKPLATFORM:
		{
			eEventType eventType = TimerThread::DeserializeEventType(wsa_ex->GetBuf());
			DEBUGMSGNOPARAM("break platform\n");
			int roomID = TimerThread::DeserializeReceiver(wsa_ex->GetBuf());
			int objectID = client_id;
			auto sPacket = mMainServer->make_game_breakdoor_packet(objectID);
			mMainServer->SendRoomBroadCast(roomID, (void*)&sPacket, sizeof(sPacket));
			break;
		}
		}
	}
}

void WorkerThread::matchTimerType(const eEventType eventType)
{
	switch (eventType)
	{
	case eEventType::TYPE_BROADCAST_ALL:
		break;
	case eEventType::TYPE_BROADCAST_ROOM:
		break;
	case eEventType::TYPE_SELF_EXCEPT:
		break;
	case eEventType::TYPE_SELF:
		break;
	case eEventType::TYPE_TARGET:
		break;
	}
}

