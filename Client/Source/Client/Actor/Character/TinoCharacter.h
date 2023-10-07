﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Character/BaseCharacter.h"
#include "MenuUI/InGameUIWidget.h"
#include "TinoCharacter.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMovementState
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Up UMETA(DisplayName = "Up"),
	EMS_Fall UMETA(DisplayName = "Fall"),
	EMS_Tumbled UMETA(DisplayName = "Tumbled"),
	EMS_Dive UMETA(DisplayName = "Dive"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class CLIENT_API ATinoCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ATinoCharacter();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

public:

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void Dive();

	void PlayTumbleMontage();

	void DisableInputMode();
	void EnableInputMode();

	void DiveBegin();
	void DiveEnd();

	void OnAccelEffect();
	void OffAccelEffect();
	
	void TimerStart();
	void TimerEnd();

	void MakeAndShowHUD();	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<UInGameUIWidget> InGameWidgetClass;
	UPROPERTY()
	UInGameUIWidget* InGameWidgetInstance = nullptr;


	void MakeAndShowDialog();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UDialogUIWidget> DialogWidgetClass;
	UPROPERTY()
	class UDialogUIWidget* DialogWidget = nullptr;

	FORCEINLINE void SetMovementState(EMovementState State) { MovementState = State; }
	FORCEINLINE void SetMaxTumbleTime(float MaxTime) { MaxTumbledTime = MaxTime; }
	FORCEINLINE float GetMaxTumbleTime() { return MaxTumbledTime; }
	FORCEINLINE bool IsDivining() { return bIsDiving; }

private:
	//키입력 관련 함수
	void OnMoveForward(float Axis);
	void OnMoveRight(float Axis);
	void OnHorizonLock(float Axis);
	void OnVerticalLock(float Axis);

	virtual void StopJumping() override;

	void CreateDummy();

private:

	bool CanMove();
	bool CanDive();

	bool CanTumble(float DeltaTime);
	void PlayTumbleMontage(float DeltaTime);

	void Align();

public:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* TumbleMontage;

private:

	UPROPERTY(VisibleDefaultsOnly)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		class UAnimMontage* DiveMontage;

	UPROPERTY(VisibleAnywhere, Category = "Animation")
		float CurrentTumbledTime;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		float MaxTumbledTime;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = true, ToolTip="비네트 강도"), Category = "Effect")
		float CustomVignetteIntensity;

	UPROPERTY(VisibleAnywhere, Category = "Enums")
		EMovementState MovementState;

	bool bCanTumbled;
	UPROPERTY(VisibleAnywhere, Category = "Animation")
		bool bIsDiving;

	FTimerHandle InGameUITimerHandle;
};
