#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		FVector Velocity;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		float Speed;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		float Direction;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		bool bIsAir;

private:
	UPROPERTY()
		class ACharacter* OwnerCharacter;
};
