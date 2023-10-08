// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Level/CustomLevel_Test.h"
#include "Network/Network.h"
#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"

#include "Global.h"


void ACustomLevel_Test::BeginPlay() {

	Super::BeginPlay();
	Network::GetNetwork()->bLevelOpenTriggerEnabled = false;	//���� ���۵����� Ʈ���� ����.
	/*
	1. ĳ���͸� �����ϰ� Network mMyCharacter�� ����
	2. ������ ������
	3. �α��� ui
	4. �α��� ������ ui������ �κ� ����.
	*/
	FName path = TEXT("Blueprint'/Game/Characters/Tino/BP_TinoCharacter.BP_TinoCharacter_C'"); //_C�� �� �ٿ��� �ȴٰ� ��.
	UClass* GeneratedCharacterBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
	FTransform trans;
	srand(time(NULL));
	mMyCharacterSpawnPosition.Y += (rand() % 10) * 10;
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
			int option = TRUE;               //���̱� �˰���� on/off
			setsockopt(Network::GetNetwork()->s_socket,             //�ش� ����
				IPPROTO_TCP,          //������ ����
				TCP_NODELAY,          //���� �ɼ�
				(const char*)&option, // �ɼ� ������
				sizeof(option));      //�ɼ� ũ��
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
	// 이미 연결 되어있다면,
	if (true == Network::GetNetwork()->bIsConnectedLobby)
	{
		auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (TinoController == nullptr)
		{
			CLog::Log("TinoController is nullptr, Connect Lobby Against Is Fail");
		}
		else {
			TinoController->ChangeMenuWidget(TinoController->GetLobbyWidgetClass());
			// 여기서 결과 UI를 띄워줌
			if (-1 != Network::GetNetwork()->GameResult.point)
			{
				UE_LOG(LogTemp, Error, TEXT("Result is Comming"));
				// UI에 3개값을 넣고 띄워주면 됨. rank는 -1이나 0이면 retire임으로, 변환해주시길 -수민-
				Network::GetNetwork()->GameResult.point;
				Network::GetNetwork()->GameResult.grade;
				Network::GetNetwork()->GameResult.rank;

				// 결과창 출력
				ShowGameResult();

				Network::GetNetwork()->GameResult = sGameResult{}; // 결과 처리 했으니 비워주기.
			}
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

void ACustomLevel_Test::ShowGameResult()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	int rank = Network::GetNetwork()->GameResult.rank;
	double grade = Network::GetNetwork()->GameResult.grade;
	int point = Network::GetNetwork()->GameResult.point;

	TinoController->ShowGameResult(rank, grade, point);
}










