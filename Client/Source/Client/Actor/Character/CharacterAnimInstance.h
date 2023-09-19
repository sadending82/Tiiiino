// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		float Speed;
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Animation")
		float Direction;

public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
		class ACharacter* OwnerCharacter;
};
