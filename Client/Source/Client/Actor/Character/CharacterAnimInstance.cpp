#include "Actor/Character/CharacterAnimInstance.h"
#include "Actor/Character/TinoCharacter.h"
#include "Global.h"

#include "GameFramework/CharacterMovementComponent.h"

void UCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		Velocity = OwnerCharacter->GetVelocity();
		Speed = OwnerCharacter->GetVelocity().Size2D();
		Direction = UKismetAnimationLibrary::CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetControlRotation());
		bIsAir = OwnerCharacter->GetCharacterMovement()->IsFalling();
	}
}