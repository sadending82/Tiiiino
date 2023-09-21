// Fill out your copyright notice in the Description page of Project Settings.

#include "Network.h"
#include "Actor/Character/TinoCharacter.h"
//void INetwork::SendPacket()
//{
//	PACKET p;
//	m_pSocket->SendPacket(reinterpret_cast<char*>(&p));
//}


void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag);
void CALLBACK recv_Gamecallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
//void CALLBACK recv_Lobbycallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);
//void CALLBACK recv_Aicallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD flag);

using namespace std;

Network::Network()
	: mMyCharacter(nullptr)
	, mGeneratedID(0)
	, isInit(false)
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

void send_login_packet(SOCKET& sock)
{
	CS_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_LOGIN;
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

	int to_process_data = num_bytes + Game->mMyCharacter->_prev_size;
	unsigned char* packet = over->GetBuf();
	int packet_size = packet[0];
	while (packet_size <= to_process_data) {
		Game->process_packet(packet);
		to_process_data -= packet_size;
		packet += packet_size;
		if (to_process_data > 0) packet_size = packet[0];
		else break;
	}
	Game->mMyCharacter->_prev_size = to_process_data;
	if (to_process_data > 0)
	{
		memcpy(over->GetBuf(), packet, to_process_data);
	}
	Game->mMyCharacter->RecvPacket();
}
