#include "Actor/Character/BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
: _prev_size(0)
, ClientID(-1)
, OverID(-1)
, ServerSyncElapsedTime(0)
, ServerSyncDeltaTime(0.016)
, s_socket(INVALID_SOCKET)
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



void ABaseCharacter::RecvPacket()
{

}


bool ABaseCharacter::ConnectServer()
{
	return true;
}
