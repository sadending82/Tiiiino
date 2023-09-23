// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Level/CustomBaseLevel.h"
#include "CustomLevel_Stage1.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ACustomLevel_Stage1 : public ACustomBaseLevel
{
	GENERATED_BODY()
public:
	virtual bool Conn() override;
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};
