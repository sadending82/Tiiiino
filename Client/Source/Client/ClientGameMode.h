// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ClientGameMode.generated.h"

enum class EBGMType : uint8;

UCLASS(minimalapi)
class AClientGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AClientGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Class")
		TSubclassOf<class ASoundManager> SoundMangerClass;

	UPROPERTY(EditDefaultsOnly, Category = "LevelName")
	TMap<FString, EBGMType> ConvertBGMTypeMap;
};



