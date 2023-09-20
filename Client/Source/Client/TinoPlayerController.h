// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "cSocket.h"
#include "Actor/Character/TinoCharacter.h"
#include "TinoPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ATinoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATinoPlayerController();
	~ATinoPlayerController();

	UFUNCTION(BlueprintPure, Category = "Properties")
	int GetSessionId();

	UPROPERTY(EditAnyWhere, Category = "Spawning")
	TSubclassOf<class ACharacter> OtherPlayer;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
	AActor* FindPlayerBySessionId(TArray<AActor*> ActorArray, const int SessionId);

	void RecvNewPlayer(cCharacter* NewPlayer);
private:
	cSocket* m_socket;
	bool m_isConnectedWithServer;
	int m_sessionId;
	
	void SendPlayerData();
	void UpdatePlayerData(const cCharacter& data);

	FTimerHandle m_playerDataHandle;


	// 새로운 플레이어가 들어올 때
	int nPlayers;
	bool m_isNewPlayerEntered;
	cCharacter* m_newPlayer;
	void UpdateNewPlayer();



	
};
