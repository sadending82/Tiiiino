#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "GameFramework/PlayerController.h"
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
		ChangeMenuWidget(StartingWidgetClass);
	}
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
