// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Character/BaseCharacter.h"
#include "TinoCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ATinoCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ATinoCharacter();

public:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

<<<<<<< Updated upstream
=======
	virtual void Tick(float DeltaTime) override;

public:

	virtual void Jump() override;
	void Dive();

	void PlayTumbleMontage();

	void DisableInputMode();
	void EnableInputMode();

	void DiveBegin();
	void DiveEnd();
	
	FORCEINLINE void SetMovementState(EMovementState State) { MovementState = State; }
	FORCEINLINE void SetMaxTumbleTime(float MaxTime) { MaxTumbledTime = MaxTime; }
	FORCEINLINE float GetMaxTumbleTime() { return MaxTumbledTime; }
	FORCEINLINE bool IsDivining() { return bIsDiving; }

>>>>>>> Stashed changes
private:
	//키입력 관련 함수
	void OnMoveForward(float Axis);
	void OnMoveRight(float Axis);
	void OnHorizonLock(float Axis);
	void OnVerticalLock(float Axis);

	void OnRunning();
	void OffRunning();

	virtual void Jump() override;
	virtual void StopJumping() override;

	void CreateDummy();

private:

<<<<<<< Updated upstream
=======
	bool CanMove();
	bool CanDive();

	bool CanTumble(float DeltaTime);
	void PlayTumbleMontage(float DeltaTime);

	void Align();



private:

>>>>>>> Stashed changes
	UPROPERTY(VisibleDefaultsOnly)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* Camera;
};
