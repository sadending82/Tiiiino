// Fill out your copyright notice in the Description page of Project Settings.

#include "Network.h"
#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"


void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
void CALLBACK recv_Lobbycallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

using namespace std;

Network::Network()
	: mMyCharacter()
	, mOtherCharacter()
	, mGeneratedID(0)
	, isInit(false)
	, bIsConnected(false)
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		mOtherCharacter[i] = nullptr;
	}
}

Network::~Network()
{
	WSACleanup();
}

std::shared_ptr<Network> Network::GetNetwork()
{
	if (m_Network.use_count() == 0)
	{
		m_Network = std::make_shared<Network>();
	}
	return m_Network;
}

bool Network::init()
{
	if (!isInit)
	{
		isInit = true;
		WSAStartup(MAKEWORD(2, 2), &WSAData);
		return true;
	}
	return false;
}

void Network::release()
{
	if (isInit)
	{
		mGeneratedID = 0;
		mMyCharacter->bIsConnected = 0;
		mMyCharacter = nullptr;
		for (auto& p : mOtherCharacter)
			p = nullptr;
		//shutdown(s_socket, SD_BOTH);
		closesocket(s_socket);
		WSACleanup();
		if (!bLevelOpenTriggerEnabled)
		{
			//openlevel로 인한 release가 아니라,
			//editor중지때문에 생기는 release라면 false시켜줌.
			bLoginFlag = false;
			//editor중지때문이니까 여기도 그냥 false로 다시 초기화.
			bLevelOpenTriggerEnabled = false;
		}
		isInit = false;
	}
}

const int Network::getNewId()
{
	int Newid = mGeneratedID;
	mGeneratedID++;

	return Newid;
}

void Network::error_display(int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, 0);
	//std::wcout << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

void send_login_packet(SOCKET& sock, const char* id, const char* passWord)
{
	CL_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CL_LOGIN;
	strcpy_s(packet.id, id);
	strcpy_s(packet.password, passWord);
	//strcpy_s(packet.name, TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName));
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_match_packet(SOCKET& sock)
{
	CL_MATCH_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CL_MATCH;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_movetogame_packet(SOCKET& sock, const char* id, const char* passWord, const int& roomID)
{
	CS_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_LOGIN;
	packet.roomID = roomID;
	strcpy_s(packet.name, id);
	strcpy_s(packet.passWord, passWord);
	//strcpy_s(packet.name, TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName));
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_move_packet(SOCKET& sock, const bool& inair, const float& x, const float& y, const float& z, FQuat& rotate, const float& value, const FVector& speedVec)
{
	CS_MOVE_PACKET packet;
	packet.size = sizeof(CS_MOVE_PACKET);
	packet.type = CS_MOVE;
	packet.inair = inair;
	packet.x = x;
	packet.y = y;
	packet.z = z;
	packet.rx = rotate.X;
	packet.ry = rotate.Y;
	packet.rz = rotate.Z;
	packet.rw = rotate.W;
	packet.speed = value;
	packet.sx = speedVec.X;
	packet.sy = speedVec.Y;
	packet.sz = speedVec.Z;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(CS_MOVE_PACKET), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}





void Network::process_packet(unsigned char* p)
{
	unsigned char Type = p[1];

	switch (Type)
	{
	case SC_LOGIN_OK:
	{
		SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(p);
		mMyCharacter->SetClientID(packet->id);
		//연결성공
		bIsConnected = true;
		break;
	}
	case SC_MOVE_PLAYER:
	{
		SC_MOVE_PLAYER_PACKET* packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(p);
		int move_id = packet->id;
		if (0 <= move_id && move_id < 8)
		{
			if (move_id == mMyCharacter->GetClientID())
			{
				//내가 움직인건 처리하지 않는다.
			}
			else if (mOtherCharacter[move_id] != nullptr)
			{
				//if (mOtherCharacter[move_id]->SyncInAirDelegate.IsBound() == true)
				//	mOtherCharacter[move_id]->SyncInAirDelegate.Broadcast(packet->inair);
				mOtherCharacter[move_id]->SetActorLocation(FVector(packet->x, packet->y, packet->z));
				mOtherCharacter[move_id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[move_id]->ServerSyncSpeed = packet->speed;
				//mOtherCharacter[move_id]->ServerStoreGroundSpeed = packet->speed;
				//mOtherCharacter[move_id]->CharMovingSpeed = FVector(packet->sx, packet->sy, packet->sz);
				//mOtherCharacter[move_id]->GroundSpeedd = packet->speed;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("UnExpected ID Come To PACKET_MOVE id: %d"), move_id);
		}
		break;
	}
	case SC_ADD_PLAYER:
	{
		SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p);
		int id = packet->id;
		if (nullptr != mOtherCharacter[id])
		{
			mOtherCharacter[id]->GetMesh()->SetVisibility(true);
			mOtherCharacter[id]->SetClientID(packet->id);
			//mOtherCharacter[id]->CharacterName = FString(ANSI_TO_TCHAR(packet->name));
			//mOtherCharacter[id]->skinType = packet->skintype;
			//mOtherCharacter[id]->EquipSkin();
			//mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard.push_back(ScoreInfo(mOtherCharacter[id]));
			//mMyCharacter->mInventory->mMainWidget->mScoreWidget->UpdateRank();

		}
		else {
			FName path = TEXT("Blueprint'/Game/Characters/Tino/BP_TinoCharacter.BP_TinoCharacter_C'"); //_C를 꼭 붙여야 된다고 함.
			UClass* GeneratedInventoryBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
			FTransform trans(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(888, 1566, 400));
			auto mc = mMyCharacter->GetWorld()->SpawnActorDeferred<ATinoCharacter>(GeneratedInventoryBP, trans);
			if (nullptr != mc)
			{
				mc->SpawnDefaultController();
				mc->AutoPossessPlayer = EAutoReceiveInput::Disabled;
				mc->FinishSpawning(trans);
				mOtherCharacter[id] = mc;
				mOtherCharacter[id]->GetMesh()->SetVisibility(true);
				mOtherCharacter[id]->SetClientID(packet->id);
				//mOtherCharacter[id]->CharacterName = FString(ANSI_TO_TCHAR(packet->name));
				//mOtherCharacter[id]->skinType = packet->skintype;
				//mOtherCharacter[id]->EquipSkin();
				//mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard.push_back(ScoreInfo(mOtherCharacter[id]));
				//mMyCharacter->mInventory->mMainWidget->mScoreWidget->UpdateRank();

			}
		}
		break;
	}
	default:
		break;
	}
}

void Network::l_process_packet(unsigned char* p)
{
	unsigned char Type = p[1];

	switch (Type)
	{
	case LC_LOGIN_OK:
	{
		LC_LOGIN_OK_PACKET* packet = reinterpret_cast<LC_LOGIN_OK_PACKET*>(p);
		mMyCharacter->SetClientID(packet->id);
		//연결성공
		bIsConnectedLobby = true;

		auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(mMyCharacter->GetWorld(), 0));
		TinoController->ChangeMenuWidget(TinoController->GetLobbyWidgetClass());

		break;
	}
	case LC_MATCH_RESPONSE:
	{
		LC_MATCH_RESPONSE_PACKET* packet = reinterpret_cast<LC_MATCH_RESPONSE_PACKET*>(p);
		//게임서버 연결 코드 나중에 ip랑 포트넘버도 넘겨야함.
		
		//UGameplayStatics::OpenLevel();
	}
	default:
		break;
	}
}



void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
	//cout << "send_callback is called" << endl;
	WSA_OVER_EX* once_exp = reinterpret_cast<WSA_OVER_EX*>(send_over);
	delete once_exp;
}


void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	WSA_OVER_EX* over = reinterpret_cast<WSA_OVER_EX*>(recv_over);
	auto Game = Network::GetNetwork();
	if (nullptr == Game->mMyCharacter) return;

	int to_process_data = num_bytes + Game->_prev_size;
	unsigned char* packet = over->GetBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Game->process_packet(packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Game->_prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(over->GetBuf(), packet, to_process_data);
	}
	Game->RecvPacketGame();
}

void CALLBACK recv_Lobbycallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	WSA_OVER_EX* over = reinterpret_cast<WSA_OVER_EX*>(recv_over);
	auto Game = Network::GetNetwork();
	if (nullptr == Game->mMyCharacter) return;

	int to_process_data = num_bytes + Game->l_prev_size;
	unsigned char* packet = over->GetBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Game->l_process_packet(packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Game->l_prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(over->GetBuf(), packet, to_process_data);
	}
	Game->RecvPacketLobby();
}



void Network::RecvPacketGame()
{
	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover.GetWsaOver(), sizeof(recv_expover.GetWsaOver()));

	recv_expover.GetWsaBuf().buf = reinterpret_cast<char*>(recv_expover.GetBuf() + _prev_size);
	recv_expover.GetWsaBuf().len = BUF_SIZE - _prev_size;

	int ret = WSARecv(s_socket, &recv_expover.GetWsaBuf(), 1, NULL, &recv_flag, &recv_expover.GetWsaOver(), recv_Gamecallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}

void Network::RecvPacketLobby()
{
	DWORD recv_flag = 0;
	ZeroMemory(&l_recv_expover.GetWsaOver(), sizeof(l_recv_expover.GetWsaOver()));

	l_recv_expover.GetWsaBuf().buf = reinterpret_cast<char*>(l_recv_expover.GetBuf() + l_prev_size);
	l_recv_expover.GetWsaBuf().len = BUF_SIZE - l_prev_size;

	int ret = WSARecv(l_socket, &l_recv_expover.GetWsaBuf(), 1, NULL, &recv_flag, &l_recv_expover.GetWsaOver(), recv_Lobbycallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}


bool Network::ConnectServerGame()
{
	if (bIsConnected) return false;
	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(GAMESERVERPORT);

	inet_pton(AF_INET, GAMESERVER_ADDR, &server_addr.sin_addr);
	int rt = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		//error_display(err_num);
		//system("pause");
		//UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
		//exit(0);
		closesocket(s_socket);
		return false;
	}

	DWORD recv_flag = 0;
	int ret = WSARecv(s_socket, &recv_expover.GetWsaBuf(), 1, NULL, &recv_flag, &recv_expover.GetWsaOver(), recv_Gamecallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
			return false;
		}
	}
	return true;
}

bool Network::ConnectServerLobby()
{
	if (bIsConnectedLobby) return false;
	l_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&l_server_addr, sizeof(l_server_addr));
	l_server_addr.sin_family = AF_INET;
	l_server_addr.sin_port = htons(LOBBYSERVERPORT);

	inet_pton(AF_INET, LOBBYSERVER_ADDR, &l_server_addr.sin_addr);
	int rt = connect(l_socket, reinterpret_cast<sockaddr*>(&l_server_addr), sizeof(l_server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		//error_display(err_num);
		//system("pause");
		//UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
		//exit(0);
		closesocket(l_socket);
		return false;
	}

	DWORD recv_flag = 0;
	int ret = WSARecv(l_socket, &l_recv_expover.GetWsaBuf(), 1, NULL, &recv_flag, &l_recv_expover.GetWsaOver(), recv_Lobbycallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			int err_num = WSAGetLastError();
			//error ! 
			return false;
		}
	}
	return true;
}
