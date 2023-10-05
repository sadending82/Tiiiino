#include "Actor/Character/CharacterAnimInstance.h"
#include "Actor/Character/TinoCharacter.h"
#include "Global.h"

#include "GameFramework/CharacterMovementComponent.h"

void UCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	auto Controller = OwnerCharacter->GetController();
	if (Controller == nullptr) return;
	bIsPossess = Controller->IsPlayerController();
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		auto Controller = OwnerCharacter->GetController();
		if (Controller == nullptr) return;
		if ( Controller->IsPlayerController()) {
			Velocity = OwnerCharacter->GetVelocity();
			Speed = OwnerCharacter->GetVelocity().Size2D();
			Direction = UKismetAnimationLibrary::CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetControlRotation());
			bIsAir = OwnerCharacter->GetCharacterMovement()->IsFalling();
		}
		else {
			auto otherplayer = Cast<ABaseCharacter>(OwnerCharacter);
			if (otherplayer)
			{
				Speed = otherplayer->ServerSyncSpeed;
			}
		}
	}
}
