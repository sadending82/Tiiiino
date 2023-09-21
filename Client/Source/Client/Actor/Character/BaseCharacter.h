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
	const int GetOverID() const {return OverID; }
	void SetOverID(const int id) { OverID = id; }

protected:
	//Server
	UPROPERTY(EditAnywhere,Category = "Network")
		int ClientID;
	UPROPERTY(EditAnywhere, Category = "Network")
		int OverID;	//overlapped I/O use this

public:
	// 112.152.55.49  127.0.0.1  , 112.153.53.142
	const char* SERVER_ADDR = "127.0.0.1";
public:
	//Game Server
	virtual bool ConnectServer() override;
	SOCKET s_socket;
	SOCKADDR_IN server_addr;
	WSA_OVER_EX recv_expover;
	int		_prev_size;
	virtual void RecvPacket() override;

	float ServerSyncElapsedTime;	//몇초 마다 이동 동기화 보낼것인지.
	float ServerSyncDeltaTime;		//몇초 마다 이동 동기화 보낼것인지.

	bool bIsConnected;	//임시 변수
	float ServerSyncSpeed;	//서버 동기화용 스피드.

};
