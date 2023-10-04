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

private:
	//Server
	UPROPERTY(EditAnywhere,Category = "Network")
		int ClientID;

<<<<<<< Updated upstream
=======
public:

	float ServerSyncElapsedTime;	//���� ���� �̵� ����ȭ ����������.
	float ServerSyncDeltaTime;		//���� ���� �̵� ����ȭ ����������.

	bool bIsConnected;	//�ӽ� ����
	float ServerSyncSpeed;	//���� ����ȭ�� �ִϸ��̼� ���ǵ�.
	FVector ServerCharMovingSpeed;	//���� ����ȭ ������ ����ó���� ����
>>>>>>> Stashed changes

};
