#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Network/Network.h"
#include "MenuUI/DialogUIWidget.h"
#include "MenuUI/FinishGameUIWidget.h"

#include "Global.h"

ATinoController::ATinoController()
	:MaxDummyCount(8),
	DummyOffset(200)
{
}

void ATinoController::BeginPlay()
{
	Super::BeginPlay();
	if (StartingWidgetClass != nullptr && UGameplayStatics::GetCurrentLevelName(GetWorld()) == "Lobby")
	{
		if (false == Network::GetNetwork()->bIsConnectedLobby)
		{
			ChangeMenuWidget(StartingWidgetClass);
		}
		else {
			SetInputUIMode();
		}
	}
	ChangeMenuWidget(StartingWidgetClass);
	DialogUI = Cast<UDialogUIWidget>(CreateWidget(GetWorld(), DialogUIClass));
}

void ATinoController::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ATinoController::CreateDummy()
{
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

	DialogUI->OpenInGameMenuUI();
	DialogUI->AddToViewport();
}

void ATinoController::CloseInGameUI()
{
	
	DialogUI->CloseInGameMenuUI();
	DialogUI->RemoveFromParent();
}

void ATinoController::DisconnectNetwork()
{
	// if Network is disconnected 


	DialogUI->RenderDisconnectNetworkWindow();
	DialogUI->AddToViewport();
	
}



void ATinoController::ShowGameResult(int rank, double grade, int point)
{
	FinishGameUI = Cast<UFinishGameUIWidget>(CreateWidget(GetWorld(), FinishGameUIClass));
	if (!!FinishGameUI)
	{
		FinishGameUI->ShowResult(rank, grade, point);
		FinishGameUI->AddToViewport();
	}
}
