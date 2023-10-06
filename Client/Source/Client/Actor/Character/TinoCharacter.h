// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Character/BaseCharacter.h"
#include "TinoCharacter.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Up UMETA(DisplayName = "Up"),
	EMS_Fall UMETA(DisplayName = "Fall"),
	EMS_Tumbled UMETA(DisplayName = "Tumbled"),
	EMS_Dive UMETA(DisplayName = "Dive"),
	EMS_Grabbing UMETA(DisplayName = "Grabbing"),
	EMS_IsGrabbed UMETA(DisplayName = "IsGrabbed"),
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

	//단순 재생용,서버 패킷을받아서 실행
	void PlayTumbleMontage();

	void Grab();
	void SetNormalToGrabbed();
	void SetGrabbedToNormal();


	void GrabBegin();
	void GrabOff();
	void DetectTarget();

	void DisableInputMode();
	void EnableInputMode();

	void DiveBegin();
	void DiveEnd();

	void OnAccelEffect();
	void OffAccelEffect();
	
	FORCEINLINE void SetMovementState(EMovementState State) { MovementState = State; }
	FORCEINLINE void SetMaxTumbleTime(float MaxTime) { MaxTumbledTime = MaxTime; }
	FORCEINLINE EMovementState GetMovementState() { return MovementState; }
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
	bool CanGrab();

	bool CanTumble(float DeltaTime);
	void PlayTumbleMontage(float DeltaTime);

	void Align();


	bool SendAnimPacket(int32 AnimType);

private:

	UPROPERTY(VisibleDefaultsOnly)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		class UAnimMontage* DiveMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		class UAnimMontage* TumbleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		class UAnimMontage* GrabMontage;

	UPROPERTY(VisibleAnywhere, Category = "Animation | Tumble")
		float CurrentTumbledTime;
	UPROPERTY(EditDefaultsOnly, Category = "Animation | Tumble")
		float MaxTumbledTime;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "그랩 최대 유지시간"), Category = "Animation | Grab")
		float MaxGrabTime;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "그랩 재사용시간"), Category = "Animation | Grab")
		float GrabCoolTime;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "그랩시 감속량"), Category = "Animation | Grab")
		float GrabbedSpeedRate;

	UPROPERTY(EditDefaultsOnly,meta = (ToolTip="탐지 거리"), Category = "Animation | Grab")
		float DetectDist;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "탐지 범위(반지름)"), Category = "Animation | Grab")
		float DetectRadius;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "탐지 각도(정면 부채꼴)"), Category = "Animation | Grab")
		float DetectAngle;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "잡은 대상과의 간격"), Category = "Animation | Grab")
		float TargetInterval;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = true, ToolTip="비네트 강도"), Category = "Effect")
		float CustomVignetteIntensity;

	UPROPERTY(VisibleAnywhere, Category = "Enums")
		EMovementState MovementState;

	UPROPERTY(VisibleAnywhere, Category = "Animation | Grab")
		AActor* Target;

	UPROPERTY(VisibleAnywhere, Category = "Animation")
		bool bCanTumbled;
	UPROPERTY(VisibleAnywhere, Category = "Animation")
		bool bIsDiving;

	UPROPERTY(VisibleAnywhere, Category = "Animation")
		bool bIsGrabbing;
	UPROPERTY(VisibleAnywhere, Category = "Animation")
		bool bIsGrabCoolTime;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | Grab")
		bool bShowDebugTrace;

	FTimerHandle GrabTimer;
	FTimerHandle GrabCoolTimer;
};
