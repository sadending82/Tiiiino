// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Level/CustomLevel_Test.h"
#include "Network/Network.h"
#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"
#include "Global.h"


void ACustomLevel_Test::BeginPlay() {

	Super::BeginPlay();
	Network::GetNetwork()->bLevelOpenTriggerEnabled = false;	//레벨 시작됐으니 트리거 꺼줌.
	/*
	1. 캐릭터를 스폰하고 Network mMyCharacter에 연결
	2. 서버에 연결함
	3. 로그인 ui
	4. 로그인 성공시 ui닫히고 로비 시작.
	*/
	FName path = TEXT("Blueprint'/Game/Characters/Tino/BP_TinoCharacter.BP_TinoCharacter_C'"); //_C를 꼭 붙여야 된다고 함.
	UClass* GeneratedCharacterBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform trans;
	trans.SetLocation(trans.GetLocation() + mMyCharacterSpawnPosition);
	auto mc1 = GetWorld()->SpawnActorDeferred<ATinoCharacter>(GeneratedCharacterBP, trans);
	mc1->SpawnDefaultController();
	mc1->AutoPossessPlayer = EAutoReceiveInput::Player0;
	mc1->FinishSpawning(trans);
	Network::GetNetwork()->mMyCharacter = mc1;
	Conn();

	//To Loading ...

	//To Do SomeThing ...

}


bool ACustomLevel_Test::Conn()
{
	Super::Conn();
	Network::GetNetwork()->init();

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
			CLog::Log("Connect Successfully");
			send_movetogame_packet(Network::GetNetwork()->s_socket,Network::GetNetwork()->mDBUID,
				TCHAR_TO_ANSI(*Network::GetNetwork()->MyCharacterName), 0);

			GetWorld()->GetFirstPlayerController<ATinoController>()->SetInputGameMode();
			
			return true;
		}
		else {
			CLog::Log("Connect Fail!");
		}
	}
	
	return false;
}

bool ACustomLevel_Test::ConnLobbyServer()
{
	auto player = Network::GetNetwork()->mMyCharacter;
	if (nullptr == player) return false;
	//이미 연결 되어있다면,
	if (Network::GetNetwork()->bIsConnectedLobby)
	{
		auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (TinoController == nullptr)
		{
			CLog::Log("TinoController is nullptr, Connect Lobby Against Is Fail");
		}
		else {
			TinoController->ChangeMenuWidget(TinoController->GetLobbyWidgetClass());
			CLog::Log("Connect Lobby Against Successfully");
		}
		return true;
	}


	if (true == Network::GetNetwork()->ConnectServerLobby())
	{
		GetWorld()->GetFirstPlayerController<ATinoController>()->SetInputUIMode();
		CLog::Log("Connect Lobby Successfully");
		return true;
	}
	else {

		CLog::Log("Connect Fail!");
	}

	//UE_LOG(LogTemp, Log, TEXT("Player Try Conn"));
	return false;
}










