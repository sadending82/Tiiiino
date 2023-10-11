// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClientGameMode.h"
#include "ClientCharacter.h"
#include "Global.h"

#include "Sound/SoundManager.h"

AClientGameMode::AClientGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//ASoundManager::GetSoundManager()->PlayBGM();
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->SpawnActor<ASoundManager>(SoundMangerClass);
	auto LevelName = ConvertBGMTypeMap[UGameplayStatics::GetCurrentLevelName(GetWorld())];
	ASoundManager::GetSoundManager()->SetBGM(LevelName);
	ASoundManager::GetSoundManager()->PlayBGM();
	
}
