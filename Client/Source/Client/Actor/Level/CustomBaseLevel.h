// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CustomBaseLevel.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ACustomBaseLevel : public ALevelScriptActor
{
	GENERATED_BODY()
public:
	virtual bool Conn();
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
