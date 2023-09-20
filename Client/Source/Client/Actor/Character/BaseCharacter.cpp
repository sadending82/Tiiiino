#include "Actor/Character/BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
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

