#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
enum class EMovementState : uint8;

UCLASS()
class CLIENT_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
	bool bIsAir;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
	bool bIsAirForNetwork = false;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
	bool bIsPossess = false;
protected:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		FVector Velocity;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		float Speed;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		float Direction;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		EMovementState MovementState;

private:
	UPROPERTY()
		class ACharacter* OwnerCharacter;
};
