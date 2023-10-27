#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Network/Network.h"
#include "MenuUI/DialogUIWidget.h"
#include "MenuUI/FinishGameUIWidget.h"
#include "MenuUI/InGameTimerWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "MenuUI/LoginUIWidget.h"
#include "MenuUI/CreateAccountsWidget.h"
#include "MenuUI/InGameUIWidget.h"

#include "Global.h"

ATinoController::ATinoController()
	:MaxDummyCount(8),
	DummyOffset(200)
{
}

void ATinoController::BeginPlay()
{
	Super::BeginPlay();
	
	// UI 인스턴스 초기화
	InitializeUIInstance();

	if (LoginWidgetClass != nullptr && UGameplayStatics::GetCurrentLevelName(GetWorld()) == "Lobby")
	{
		if (false == Network::GetNetwork()->bIsConnectedLobby)
		{
			if(bIsLobbyConnected)
				ChangeMenuWidget(LoginUIInstance);		
			else 
				DisconnectNetwork();
		}
		else {
			SetInputUIMode();
		}
	}
	//ChangeMenuWidget(StartingWidgetClass);
	
}

void ATinoController::ChangeMenuWidget(UUserWidget* NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = NewWidgetClass;
		CurrentWidget->AddToViewport();
		
	}
}

void ATinoController::CreateDummy()
{
	return;
	if (TinoCharacterClass)
	{
		ATinoCharacter* Dummy = GetWorld()->SpawnActor<ATinoCharacter>(TinoCharacterClass);

		if (MaxDummyCount > DummyArray.Num())
		{
			FVector Location = GetPawn()->GetActorLocation();
			FVector Dir = GetPawn()->GetActorForwardVector();
			Location += FVector(Dir.X * (DummyArray.Num()+1) * DummyOffset,0.f,0.f);
			Dummy->SetActorLocation(Location);
			DummyArray.Add(Dummy);
		}
	}
}

void ATinoController::SetInputUIMode()
{
	FInputModeUIOnly LobbyInputMode;
	SetInputMode(LobbyInputMode);
	SetShowMouseCursor(true);
}

void ATinoController::SetInputGameMode()
{
	FInputModeGameOnly GameInputMode;
	SetInputMode(GameInputMode);
	SetShowMouseCursor(false);
}

void ATinoController::OpenInGameUI()
{
	if (!!DialogUIInstance)
	{
		DialogUIInstance->RenderInGameMenuUI();
		DialogUIInstance->AddToViewport();
	}
	
}

void ATinoController::RemoveDialogUI()
{
	if (!!DialogUIInstance)
	{
		DialogUIInstance->ResetWindow();
		DialogUIInstance->RemoveFromParent();
	}
}

void ATinoController::UIAlertMessage(EDialogUICheck check)
{
	if (!!DialogUIInstance)
	{
		DialogUIInstance->FixedAlertMessage();
		if (check == EDialogUICheck::EDC_LoginFailed)
		{
			DialogUIInstance->SetLoginFailedUI();
		}
		else if (check == EDialogUICheck::EDC_CASuccess)
		{
			DialogUIInstance->SetCreateAccountSuccessUI();
		}
		else if (check == EDialogUICheck::EDC_CAFailed)
		{
			DialogUIInstance->SetCreateAccountFailedUI();
		}

		DialogUIInstance->RenderUIAlertMessage();
		DialogUIInstance->AddToViewport();
	}
}

void ATinoController::DisconnectNetwork()
{
	// if Network is disconnected 

	if (!!DialogUIInstance)
	{
		DialogUIInstance->RenderDisconnectNetworkWindow();
		DialogUIInstance->AddToViewport();
	}
}

void ATinoController::ShowGameResult(int level, int rank, double grade, int point)
{

	if (!!FinishGameUIInstance)
	{
		FinishGameUIInstance->ShowResult(level, rank, grade, point);
		FinishGameUIInstance->AddToViewport();
	}
}

void ATinoController::InitializeUIInstance()
{
	
	if (!!LoginWidgetClass)
	{
		if (LoginUIInstance == nullptr)
			LoginUIInstance = CreateWidget<ULoginUIWidget>(GetWorld(), LoginWidgetClass);
	}
	if (!!LobbyWidgetClass)
	{
		if(LobbyUIInstance == nullptr)
			LobbyUIInstance = CreateWidget<ULobbyUIWidget>(GetWorld(), LobbyWidgetClass);
	}
	if (!!CreateAccountsWidgetClass)
	{
		if (CreateAccountsUIInstance == nullptr)
			CreateAccountsUIInstance = CreateWidget<UCreateAccountsWidget>(GetWorld(), CreateAccountsWidgetClass);
	}
	if (!!DialogWidgetClass)
	{
		if (DialogUIInstance == nullptr)
			DialogUIInstance = Cast<UDialogUIWidget>(CreateWidget(GetWorld(), DialogWidgetClass));
	}
	if (!!InGameUITimerWidgetClass)
	{
		if (InGameUITimerInstance == nullptr)
			InGameUITimerInstance = CreateWidget<UInGameTimerWidget>(GetWorld(), InGameUITimerWidgetClass);
	}
	if (!!InGameWidgetClass)
	{
		if (InGameUIInstance == nullptr)
			InGameUIInstance = CreateWidget<UInGameUIWidget>(GetWorld(), InGameWidgetClass);
	}
	if (!!FinishGameUIWidgetClass)
	{
		if (FinishGameUIInstance == nullptr)
			FinishGameUIInstance = CreateWidget<UFinishGameUIWidget>(GetWorld(), FinishGameUIWidgetClass);
	}
}

void ATinoController::TimerStart(ETimerType type)
{
	Type = type;
	GetWorldTimerManager().SetTimer(UITimerHandle, this, &ATinoController::TimerRun, true, true);
}

void ATinoController::TimerRun()
{
	// 유효성 확인
	if (!!InGameUITimerInstance)
	{
		// UInGameUIWidget의 TimerRun 함수 호출
		InGameUITimerInstance->TimerRun(Type);

		if (InGameUITimerInstance->GetRestGameTime() < 0)
		{
			InGameUITimerInstance->TimerEnd(Type);
			GetWorldTimerManager().ClearTimer(UITimerHandle);
		}
	}
}
