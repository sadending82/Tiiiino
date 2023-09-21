// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TinoController.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ATinoController : public APlayerController
{
	GENERATED_BODY()

public:
	ATinoController();

public:

	void CreateDummy();

private:

	UPROPERTY(EditAnywhere, Category = "Test")
		int32 MaxDummyCount;

	UPROPERTY(EditAnywhere, Category = "Test")
		int32 DummyOffset;

	UPROPERTY(EditAnywhere, Category = "Test")
		TArray<class ATinoCharacter*> DummyArray;

	UPROPERTY(EditAnywhere, Category = "BPClass")
		TSubclassOf<class ATinoCharacter> TinoCharacterClass;

};
