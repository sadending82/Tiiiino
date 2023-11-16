// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Level/CustomLevel_Test.h"
#include "Sound/SoundManager.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Network/Network.h"
#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameDataManager/GameDataManager.h"

#include "Global.h"


void ACustomLevel_Test::BeginPlay() {

	Super::BeginPlay();
	auto Game = Network::GetNetwork();
	Game->init();
	Game->bLevelOpenTriggerEnabled = false;	//레벨 시작됐으니 트리거 꺼줌.
	/*
	1. 캐릭터를 스폰하고 Network mMyCharacter에 연결
	2. 서버에 연결함
	3. 로그인 ui
	4. 로그인 성공시 ui닫히고 로비 시작.
	*/
	FName path = TEXT("Blueprint'/Game/Characters/Tino/BP_TinoCharacter.BP_TinoCharacter_C'"); //_C를 꼭 붙여야 된다고 함.
	UClass* GeneratedCharacterBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform trans;
	srand(time(NULL));
	;
	trans.SetRotation(FRotator(0, 90, 0).Quaternion());
	//mMyCharacterSpawnPosition.Y += (rand() % 10) * 10;
	trans.SetLocation(trans.GetLocation() + mMyCharacterSpawnPosition);
	auto mc1 = GetWorld()->SpawnActorDeferred<ATinoCharacter>(GeneratedCharacterBP, trans);
	mc1->SpawnDefaultController();
	mc1->AutoPossessPlayer = EAutoReceiveInput::Player0;
	mc1->FinishSpawning(trans);
	Game->mMyCharacter = mc1;
	Conn();

	//To Loading ...

	//To Do SomeThing ...

}


bool ACustomLevel_Test::Conn()
{
	Super::Conn();

	if (bGameserverConn)
		return ConnGameServer();
	else
		return ConnLobbyServer();

}

bool ACustomLevel_Test::ConnGameServer()
{
	auto player = Network::GetNetwork()->mMyCharacter;
	if (nullptr != player)
	{
		if (true == Network::GetNetwork()->ConnectServerGame())
		{
			int option = TRUE;               //네이글 알고리즘 on/off
			setsockopt(Network::GetNetwork()->s_socket,             //해당 소켓
				IPPROTO_TCP,          //소켓의 레벨
				TCP_NODELAY,          //설정 옵션
				(const char*)&option, // 옵션 포인터
				sizeof(option));      //옵션 크기
			CLog::Log("Connect Successfully");
			send_movetogame_packet(Network::GetNetwork()->s_socket,Network::GetNetwork()->mDBUID,
				TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName), 0); 
			GetWorld()->GetFirstPlayerController<ATinoController>()->SetInputGameMode();

			Network::GetNetwork()->mMyCharacter->DisableInputMode();
			return true;
		}
		else {
			player->MakeAndShowDialogInGame();
			CLog::Log("Connect Fail!");
		}
	}
	
	return false;
}

bool ACustomLevel_Test::ConnLobbyServer()
{
	auto Game = Network::GetNetwork();
	auto player = Game->mMyCharacter;
	if (nullptr == player) return false;
	player->GetCharacterMovement()->GravityScale = 0.0;
	// 이미 연결 되어있다면,
	if (true == Game->bIsConnectedLobby)
	{
		auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (TinoController == nullptr)
		{
			CLog::Log("TinoController is nullptr, Connect Lobby Against Is Fail");
		}
		else {
			TinoController->InitializeUIInstance();
			send_refresh_dep_rank_packet(Game->l_socket);
			send_refresh_userstatus_packet(Game->l_socket);
			send_refresh_inventory_packet(Game->l_socket);

			// 여기서 결과 UI를 띄워줌
			if (-1 != Game->GameResult.point)
			{
				UE_LOG(LogTemp, Error, TEXT("Result is Comming"));
				// UI에 3개값을 넣고 띄워주면 됨. rank는 -1이나 0이면 retire임으로, 변환해주시길 -수민-
				Game->GameResult.point;
				Game->GameResult.grade;
				Game->GameResult.rank;

				//학점 재반영 (게임 종료 -> 로비)
				player->MakeAndShowLoginOK(Game->GameResult.grade);
				// 결과창 출력
				ShowGameResult();
				Game->GameResult = sGameResult{}; // 결과 처리 했으니 비워주기.
			}

			CLog::Log("Connect Lobby Against Successfully");
		}
		return true;
	}


	if (true == Network::GetNetwork()->ConnectServerLobby())
	{
		send_checkversion_packet(Game->l_socket, GAMEVERSION);
		GetWorld()->GetFirstPlayerController<ATinoController>()->SetInputUIMode();
		CLog::Log("Connect Lobby Successfully");
		auto controller = Cast<ATinoController>(player->GetController());
		if (!!controller)
		{
			controller->bIsLobbyConnected = true;
		}
		return true;
	}
	else {
		CLog::Log("Connect Fail!");
	}

	//UE_LOG(LogTemp, Log, TEXT("Player Try Conn"));
	return false;
}

void ACustomLevel_Test::ShowGameResult()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!!TinoController)
	{
		int level = Network::GetNetwork()->RecentLevelNum;
		int rank = Network::GetNetwork()->GameResult.rank;
		double grade = Network::GetNetwork()->GameResult.grade;
		int point = Network::GetNetwork()->GameResult.point;

		TinoController->ShowGameResult(level, rank, grade, point);
	}
}













