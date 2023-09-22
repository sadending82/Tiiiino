#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Network.h"
#include "BaseCharacter.generated.h"

UCLASS()
class CLIENT_API ABaseCharacter : public ACharacter, public INetwork
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	const int GetClientID() const { return ClientID; }
	void SetClientID(const int id) { ClientID = id; }

protected:
	//Server
	UPROPERTY(EditAnywhere,Category = "Network")
		int ClientID;

public:

	float ServerSyncElapsedTime;	//몇초 마다 이동 동기화 보낼것인지.
	float ServerSyncDeltaTime;		//몇초 마다 이동 동기화 보낼것인지.

	bool bIsConnected;	//임시 변수
	float ServerSyncSpeed;	//서버 동기화용 스피드.

};
