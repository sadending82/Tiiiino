// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Level/CustomBaseLevel.h"
#include "CustomLevel_Test.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ACustomLevel_Test : public ACustomBaseLevel
{
	GENERATED_BODY()
public:
	virtual bool Conn() override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




private:
	bool ConnGameServer();
	bool ConnLobbyServer();

public:
	UPROPERTY(EditAnywhere, Category = "Network")
	bool bGameserverConn = true;	// true - gameserver connect / false - lobbyserver connect
	UPROPERTY(EditAnywhere, Category = "Character")
	FVector mMyCharacterSpawnPosition;	//

};
