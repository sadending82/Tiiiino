// Fill out your copyright notice in the Description page of Project Settings.

#include "Network.h"
#include <string>
#include "Actor/Character/TinoCharacter.h"
#include "Actor/Obstacles/BaseObstacle.h"
#include "Actor/Controller/TinoController.h"
#include "CreateAccountsWidget.h"
#include "MenuUI/InGameUIWidget.h"
#include "LoginUIWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Actor/Character/CharacterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/CLog.h"


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
	, l_socket(INVALID_SOCKET)
	, s_socket(INVALID_SOCKET)
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		mOtherCharacter[i] = nullptr;
	}
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		mObjects[i] = nullptr;
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

void Network::RegisterObjectNetwork(ABaseObstacle* object, int& ObjectID)
{
	mObjects[ObjectID] = object;
	//if (mObjects[mObjectCnt])
	//{
	//	if (!IsValid(mObjects[mObjectCnt]))
	//	{
	//		mObjects[mObjectCnt]->SetIsObjectSync(false);
	//		mObjects[mObjectCnt] = object;
	//		ObjectID = mObjectCnt;
	//		mObjectCnt++;
	//	}
	//}
	//else {
	//	mObjects[mObjectCnt] = object;
	//	ObjectID = mObjectCnt;
	//	mObjectCnt++;
	//}
}

void Network::SetObjectNetID(ABaseObstacle* object, const int netID)
{
	mObjects[netID] = object;
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
		bGameIsStart = false;
		mGeneratedID = 0;
		mMyCharacter->bIsConnected = 0;
		mMyCharacter = nullptr;
		for (auto& p : mOtherCharacter)
			p = nullptr;
		for (auto& p : mObjects)
			p = nullptr;
		mObjectCnt = 0;
		bGameEndFlag = false;
		//shutdown(s_socket, SD_BOTH);
		closesocket(s_socket);
		_prev_size = 0;
		WSACleanup();
		if (!bLevelOpenTriggerEnabled)
		{
			//openlevel로 인한 release가 아니라,
						//editor중지때문에 생기는 release라면 false시켜줌.
			bLoginFlag = false;
			//editor중지때문이니까 여기도 그냥 false로 다시 초기화.
			bLevelOpenTriggerEnabled = false;
			//editor 중지가 아니라 level 변경시 불리는 release에서 변경되지 말아야 할 값은 이 if문 안에 넣기.
			bIsConnectedLobby = 0;
			bIsConnected = 0;
			closesocket(l_socket);
			l_socket = INVALID_SOCKET;
			l_prev_size = 0;
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

void send_newaccount_packet(SOCKET& sock, const char* id, const char* passWord, const int department)
{
	CL_SIGNUP_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CL_SIGNUP;
	packet.department = department;
	strcpy_s(packet.id, id);
	strcpy_s(packet.password, passWord);
	//strcpy_s(packet.name, TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName));
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_login_packet(SOCKET& sock, const char* id, const char* passWord)
{
	CL_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CL_LOGIN;
	strcpy_s(packet.id, id);
	strcpy_s(packet.password, passWord);
	strcpy_s(packet.gameVersion, GAMEVERSION);
	//strcpy_s(packet.name, TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName));
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_logout_packet(SOCKET& sock)
{
	CL_LOGOUT_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CL_LOGOUT;
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

void send_matchout_packet(SOCKET& sock)
{
	CL_MATCH_OUT_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CL_MATCH_OUT;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_control_packet(SOCKET& sock)
{
	CL_CONTROL_PACKET packet;
	packet.size = sizeof(packet);
	//packet.type = CL_CONTROL;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
			auto Game = Network::GetNetwork();
			if (Game->bIsConnectedLobby)
			{
				Game->bIsConnectedLobby = false;
				if (Game->mMyCharacter)
					Game->mMyCharacter->MakeAndShowDialogInLobby();
			}
		}
	}
}

void send_movetogame_packet(SOCKET& sock, const int uID, const char* id, const int& roomID)
{
	CS_LOGIN_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_LOGIN;
	packet.roomID = roomID;
	packet.uID = uID;
	strcpy_s(packet.name, id);
	strcpy_s(packet.hashs, Network::GetNetwork()->hashs);
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
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
			auto Game = Network::GetNetwork();
			if (Game->bIsConnected)
			{
				Game->bIsConnected = false;
				if (Game->mMyCharacter)
					Game->mMyCharacter->MakeAndShowDialogInGame();
			}
		}
		else {
		}
	}
}

void send_action_packet(SOCKET& sock, const char action)
{
	CS_ACTION_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_ACTION;
	packet.action = action;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_ping_packet(SOCKET& sock, const long long ping)
{
	CS_PING_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_PING;
	packet.ping = ping;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_goal_packet(SOCKET& sock)
{
	CS_GOAL_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_GOAL;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_game_playerload_ack_packet(SOCKET& sock)
{
	CS_GAME_PLAYERLOAD_ACK_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_GAME_PLAYERLOAD_ACK;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_game_breakdoor_packet(SOCKET& sock, const int objectID)
{
	CS_GAME_BREAKDOOR_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_GAME_BREAKDOOR;
	packet.objectID = objectID;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
	int ret = WSASend(sock, &once_exp->GetWsaBuf(), 1, 0, 0, &once_exp->GetWsaOver(), send_callback);
}

void send_game_breakplatform_packet(SOCKET& sock, const int objectID)
{
	CS_GAME_BREAKPLATFORM_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_GAME_BREAKPLATFORM;
	packet.objectID = objectID;
	WSA_OVER_EX* once_exp = new WSA_OVER_EX(sizeof(packet), &packet);
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
		mMyCharacter->SetDepartmentClothes(packet->department);
		FVector location(240.f - ((packet->id % 4) * 160), 0 - (packet->id / 4 * 160), 0);
		mMyCharacter->SetActorLocation(location);
		//연결성공
		bIsConnected = true;
		break;
	}
	case SC_GAME_PLAYERLOAD_OK:
	{
		SC_GAME_PLAYERLOAD_OK_PACKET* packet = reinterpret_cast<SC_GAME_PLAYERLOAD_OK_PACKET*>(p);

		UE_LOG(LogTemp, Error, TEXT("PLAYERLOAD PACKET COME IN"));
		send_game_playerload_ack_packet(s_socket);
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
				mOtherCharacter[move_id]->ServerLocateLerp = FVector(packet->x, packet->y, packet->z);
				mOtherCharacter[move_id]->ServerRotateLerp = FQuat(packet->rx, packet->ry, packet->rz, packet->rw);
				//FMath::Lerp(mOtherCharacter[move_id]->GetActorLocation(), FVector(packet->x, packet->y, packet->z),0.5);
				////mOtherCharacter[move_id]->SetActorLocation(FVector(packet->x, packet->y, packet->z));
				mOtherCharacter[move_id]->SetActorLocation(FVector(packet->x, packet->y, packet->z));
				mOtherCharacter[move_id]->SetActorRotation(FQuat(packet->rx, packet->ry, packet->rz, packet->rw));
				mOtherCharacter[move_id]->ServerSyncSpeed = packet->speed;
				mOtherCharacter[move_id]->ServerCharMovingSpeed = FVector(packet->sx, packet->sy, packet->sz);
				mOtherCharacter[move_id]->SetIsAirForNetwork(packet->inair);
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
		//나와 같은 아이디라면 또 만들어줄 이유 없음. 탈출.
		if (id == mMyCharacter->GetClientID())
			break;
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
			FTransform trans(FQuat(packet->rx, packet->ry, packet->rz, packet->rw), FVector(888, 1566, -10000 + (id * 150)));
			auto mc = mMyCharacter->GetWorld()->SpawnActorDeferred<ATinoCharacter>(GeneratedInventoryBP, trans);
			if (nullptr != mc)
			{
				mc->SpawnDefaultController();
				mc->AutoPossessPlayer = EAutoReceiveInput::Disabled;
				mc->bIsControlledPlayer = false;
				mc->FinishSpawning(trans);
				mc->SetDepartmentClothes(packet->department);
				mOtherCharacter[id] = mc;
				mOtherCharacter[id]->GetMesh()->SetVisibility(true);
				mOtherCharacter[id]->SetClientID(packet->id);
				mOtherCharacter[id]->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Empty"));
				//mOtherCharacter[id]->CharacterName = FString(ANSI_TO_TCHAR(packet->name));
				//mOtherCharacter[id]->skinType = packet->skintype;
				//mOtherCharacter[id]->EquipSkin();
				//mMyCharacter->mInventory->mMainWidget->mScoreWidget->ScoreBoard.push_back(ScoreInfo(mOtherCharacter[id]));
				//mMyCharacter->mInventory->mMainWidget->mScoreWidget->UpdateRank();

			}
		}
		break;
	}
	case SC_PLAYER_REMOVE:
	{
		SC_PLAYER_REMOVE_PACKET* packet = reinterpret_cast<SC_PLAYER_REMOVE_PACKET*>(p);
		int id = packet->id;
		if (nullptr != mOtherCharacter[id])
		{
			mOtherCharacter[id]->Destroy();
			mOtherCharacter[id] = nullptr;
		}
		break;
	}
	case SC_PING: {
		SC_PING_PACKET* packet = reinterpret_cast<SC_PING_PACKET*>(p);
		send_ping_packet(s_socket, packet->ping);

		break;
	}
	case SC_ACTION_ANIM: {
		SC_ACTION_ANIM_PACKET* packet = reinterpret_cast<SC_ACTION_ANIM_PACKET*>(p);
		int id = packet->id;
		if (nullptr == mOtherCharacter[id])
		{
		}
		else {
			switch (packet->action)
			{
			case 1:
				mOtherCharacter[id]->Jump();
				//점프
				break;
			case 2:
				mOtherCharacter[id]->Dive();
				//다이브
				break;
			case 3:
				mOtherCharacter[id]->PlayTumbleMontage();
				//착지(텀블)
				//mOtherCharacter[id]->Dive();
				break;
			case 4:
				mOtherCharacter[id]->OnGrab();
				break;
				//잡기
			case 5:
				mOtherCharacter[id]->OffGrab();
				//잡기취소
				break;

			default:
				break;
			}
		}
		break;
	}
	case SC_GAME_WAITTING: {
		SC_GAME_WAITTING_PACKET* packet = reinterpret_cast<SC_GAME_WAITTING_PACKET*>(p);
		bGameIsStart = true;
		mMyCharacter->MakeAndShowHUD();
		for (auto obj : mObjects)
		{
			if (obj)
				obj->EnableMoveStart(true);
		}
		mMyCharacter->InGameWidgetInstance->LevelStartCountdown();
		//
		// 카운트다운 UI 띄우기및 object들 처음 동기화.
		//
		break;
	}
	case SC_GAME_START: {
		SC_GAME_START_PACKET* packet = reinterpret_cast<SC_GAME_START_PACKET*>(p);
		mMyCharacter->InGameWidgetInstance->LevelStart();
		mMyCharacter->EnableInputMode();
		//
		// 플레이어들 움직일 수 있게 하기.
		//
		break;
	}
	case SC_GAME_END: {
		SC_GAME_END_PACKET* packet = reinterpret_cast<SC_GAME_END_PACKET*>(p);
		bGameEndFlag = true;
		closesocket(s_socket);
		mMyCharacter->InGameWidgetInstance->ShowResultUI();

		break;
	}
	case SC_PLAYER_ARRIVE:
	{
		SC_PLAYER_ARRIVE_PACKET* packet = reinterpret_cast<SC_PLAYER_ARRIVE_PACKET*>(p);
		if (nullptr != mOtherCharacter[packet->id])
		{
			mOtherCharacter[packet->id]->SetActorEnableCollision(false);
			mOtherCharacter[packet->id]->SetActorHiddenInGame(true);
			//
			//UI에 도착한 인원 +1 Update
			//
		}
		break;
	}
	case SC_GAME_COUNTDOWN_START:
	{
		SC_GAME_COUNTDOWN_START_PACKET* packet = reinterpret_cast<SC_GAME_COUNTDOWN_START_PACKET*>(p);

		mMyCharacter->InGameWidgetInstance->LevelClearCountdown();
		//카운트다운 UI 띄우기 (Appear CountDown UI)

		break;
	}
	case SC_GAME_BREAKDOOR:
	{
		SC_GAME_BREAKDOOR_PACKET* packet = reinterpret_cast<SC_GAME_BREAKDOOR_PACKET*>(p);

		mObjects[packet->objectID]->ActionObject();
		break;
	}
	case SC_GAME_BREAKPLATFORM:
	{
		SC_GAME_BREAKPLATFORM_PACKET* packet = reinterpret_cast<SC_GAME_BREAKPLATFORM_PACKET*>(p);
		mObjects[packet->objectID]->ActionObject();

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
		mDBUID = packet->UID;
		//연결성공
		bIsConnectedLobby = true;
		CLog::Print("LC_LOGIN_OK IS CALLING");

		auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(mMyCharacter->GetWorld(), 0));
		if (TinoController == nullptr)
		{
			CLog::Print("TinoController is nullptr");
		}
		TinoController->ChangeMenuWidget(TinoController->GetLobbyWidgetClass());

		break;
	}
	case LC_LOGIN_FAIL:
	{
		LC_LOGIN_FAIL_PACKET* packet = reinterpret_cast<LC_LOGIN_FAIL_PACKET*>(p);

		// 로그인 실패 UI 띄움
		if (mMyCharacter->LoginUIInstance == nullptr)
			mMyCharacter->SetLoginUIInstance();
		mMyCharacter->LoginUIInstance->UIAlertMessage();

		break;
	}
	case LC_SIGNUP_OK:
	{
		LC_SIGNUP_OK_PACKET* packet = reinterpret_cast<LC_SIGNUP_OK_PACKET*>(p);

		// 회원가입 성공 UI 띄움
		if (mMyCharacter->CreateAccountsInstance == nullptr)
			mMyCharacter->SetCreateAccountsInstance();
		mMyCharacter->CreateAccountsInstance->CheckCreateAccount(true);

		break;
	}
	case LC_SIGNUP_FAIL:
	{
		LC_SIGNUP_FAIL_PACKET* packet = reinterpret_cast<LC_SIGNUP_FAIL_PACKET*>(p);

		// 회원가입 실패 UI 띄움
		if (mMyCharacter->CreateAccountsInstance == nullptr)
			mMyCharacter->SetCreateAccountsInstance();
		mMyCharacter->CreateAccountsInstance->CheckCreateAccount(false);

		break;
	}
	case LC_MATCH_RESPONSE:
	{
		LC_MATCH_RESPONSE_PACKET* packet = reinterpret_cast<LC_MATCH_RESPONSE_PACKET*>(p);
		//게임서버 연결 코드 나중에 ip랑 포트넘버도 넘겨야함.
		UE_LOG(LogTemp, Error, TEXT("Game Match Responed"));
		string maplv{ "Level" };
		RecentLevelNum = packet->mapLevel;
		maplv += std::to_string(packet->mapLevel);
		UGameplayStatics::OpenLevel(mMyCharacter->GetWorld(), FName(maplv.c_str()));
		strcpy_s(hashs, packet->hashs);
		bLevelOpenTriggerEnabled = true;
		break;
	}
	case LC_GAME_RESULT: {
		LC_GAME_RESULT_PACKET* packet = reinterpret_cast<LC_GAME_RESULT_PACKET*>(p);
		GameResult.rank = packet->rank;
		GameResult.grade = packet->grade;
		GameResult.point = packet->point;
		break;
	}
	case LC_CONTROL: {
		//send_control_packet(l_socket);
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
	if (err != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[%d]"), err);
		if (Game->mMyCharacter)
			Game->mMyCharacter->MakeAndShowDialogInGame();
		Game->release();
		return;
	}
	if (nullptr == Game->mMyCharacter) return;
	if (num_bytes == 0)return;

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
	if (err != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[%d]"), err);
		if (Game->mMyCharacter)
			Game->mMyCharacter->MakeAndShowDialogInLobby();
		Game->release();
		return;
	}
	if (nullptr == Game->mMyCharacter) return;
	if (num_bytes == 0)return;

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



bool Network::RecvPacketGame()
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
			UE_LOG(LogTemp, Error, TEXT("return false"));
			mMyCharacter->MakeAndShowDialogInGame();
			return false;
		}
		else {
			//UE_LOG(LogTemp, Error, TEXT("return true"));
		}
	}
	return true;
}

bool Network::RecvPacketLobby()
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
			UE_LOG(LogTemp, Error, TEXT("return false"));
			mMyCharacter->MakeAndShowDialogInLobby();
			return false;
		}
		else {
			//UE_LOG(LogTemp, Error, TEXT("return true"));
		}
	}
	return true;
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
	return RecvPacketGame();
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

	return RecvPacketLobby();
}