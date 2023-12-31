#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class CLIENT_API ABaseCharacter : public ACharacter
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
	float ServerSyncSpeed;	//서버 동기화용 애니메이션 스피드.
	FVector ServerCharMovingSpeed;	//서버 동기화 움직임 보간처리용 변수
	FVector ServerLocateLerp;		//서버 동기화 pos 보간
	FQuat ServerRotateLerp;		//서버 동기화 rot 보간
	bool bIsControlledPlayer = true;		//껍데기인지 플레이어인지 구분하는 변수(네트워크 종료를 위함) 껍데기는 false, 기본은 true

};
