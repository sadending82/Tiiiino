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
	EMS_GameEnd UMETA(DisplayName = "GameEnd"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EDepartment : uint8
{
	EDepartment_None UMETA(DisplayName = "None"),
	EDepartment_Game UMETA(DisplayName = "Game"),
	EDepartment_Ai UMETA(DisplayName = "Ai"),
	EDepartment_Computer UMETA(DisplayName = "Computer"),
	EDepartment_MechanicalEngine UMETA(DisplayName = "MechanicalEngine"),
	EDepartment_MechanicalDesign UMETA(DisplayName = "MechanicalDesign"),
	EDepartment_Mechatornics UMETA(DisplayName = "Mechatornics"),
	EDepartment_Electrionics UMETA(DisplayName = "Electrionics"),
	EDepartment_AdvMaterial UMETA(DisplayName = "AdvMaterial"),
	EDepartment_Chemical UMETA(DisplayName = "Chemical"),
	EDepartment_Nano UMETA(DisplayName = "Nano"),
	EDepartment_EnergyElec UMETA(DisplayName = "EnergyElec"),
	EDepartment_Bussiness UMETA(DisplayName = "Bussiness"),
	EDepartment_Design UMETA(DisplayName = "Design"),
	EDepartment_Staff UMETA(DisplayName = "Staff"),
	EDepartment_MAX UMETA(DisplayName = "DefaultMAX")
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

	//상속 함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;

	virtual void Landed(const FHitResult& Hit) override;

public:

	//다이브 관련 함수
	UFUNCTION(BlueprintCallable)
	void Dive();

	void DiveBegin();
	void DiveEnd();

	//단순 재생용,서버 패킷을받아서 실행
	void PlayTumbleMontage();

	//잡기 관련 함수
	void OnGrab();
	void OffGrab();

	void SetTargetNormalToGrabbed();
	void SetTargetGrabbedToNormal();

	void DetectTarget();
	void GrabBegin();

	void DisableInputMode();
	void EnableInputMode();

	//외부 호출 함수(장애물)
	void OnAccelEffect();
	void OffAccelEffect();

	void SetOriginalSpeed();

	//UI 함수
	void MakeAndShowHUD();	

	void MakeAndShowLoginOK(const double GradeValue, const int PointValue);
	void MakeAndShowLoginFail();
	void MakeAndShowCreateAccountsSignUpOK();
	void MakeAndShowCreateAccountsSignUpFail();

	void MakeAndShowLevelStartCountdown();
	void MakeAndShowInGameLevelStart();
	void MakeAndShowInGameLevelClear();
	void MakeAndShowInGameShowResult();
	void MakeAndShowInGameClearCountdown();

	void MakeAndShowDialogInLobby();
	void MakeAndShowDialogInGame();

	//Accessory

	//소켓 이름과 메시를 정해줘야함
	UFUNCTION(Blueprintcallable, Category = "Accessory")
		void WearAccessory();

	//임시 나중에 상점 생기면 수정할것
	UPROPERTY(EditDefaultsOnly, Category = "Accessory")
	TArray<TSubclassOf<class AAccessoryItem>> AccessoryInvetory;

	//Getter & Setter
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMovementState(EMovementState State) { MovementState = State; }
	FORCEINLINE void SetMaxTumbleTime(float MaxTime) { MaxTumbledTime = MaxTime; }
	FORCEINLINE EMovementState GetMovementState() { return MovementState; }
	FORCEINLINE float GetMaxTumbleTime() { return MaxTumbledTime; }
	FORCEINLINE bool IsDivining() { return bIsDiving; }
	FORCEINLINE float GetOriginalWalkSpeed() { return OriginalSpeed; }
	FORCEINLINE FRotator GetOriginalRotationSpeed() { return OriginalRotationSpeed; }
	FORCEINLINE float GetGrade() const { return Grade; }
	FORCEINLINE void SetGrade(const float GradeValue) { Grade = GradeValue; };
	FORCEINLINE float GetPoint() const { return Point; }
	FORCEINLINE void SetPoint(const float PointValue) { Point = PointValue; };
	FORCEINLINE FVector GetNetworkLocation() { return PreviousLocation; }

	UFUNCTION(BlueprintCallable)
	void SetDepartmentClothes(int department);

	bool GetIsAirForNetwork();
	void SetIsAirForNetwork(bool val);
	void SetNetworkLocation(const FVector& Location);

	void SetCollisionProfileName(const FName& CollisionName);
	class UCharacterAnimInstance* GetTinoAnimInstance();





	UPROPERTY()
		bool bIsSpactateModeEnabled = false;
	UPROPERTY(EditAnywhere, Category = "Sound")
		bool bIsPlaySelf;
private:
	//키입력 관련 함수
	void OnMoveForward(float Axis);
	void OnMoveRight(float Axis);
	void OnHorizonLock(float Axis);
	void OnVerticalLock(float Axis);

	virtual void StopJumping() override;

	void CreateDummy();

private:
	
	//티노 캐릭터에서만 사용하는 함수들
	bool CanMove();
	bool CanDive();
	bool CanGrab();
	bool CanJump();
	bool CanTumble(float DeltaTime);
	//애니메이션 패킷을 전송함
	bool SendAnimPacket(int32 AnimType);

	void PlayTumbleMontage(float DeltaTime);
	void Align();

	void PlayerInterpolation(float DeltaTime);

	UTexture* GetTinoDepartTexture(EDepartment DepartmentNumber);

private:

	UPROPERTY(VisibleDefaultsOnly)
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly)
		class ATinoController* PlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		class UAnimMontage* DiveMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		class UAnimMontage* TumbleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		class UAnimMontage* GrabMontage;

	UPROPERTY(VisibleDefaultsOnly, Category = "Stat")
		float Grade;
	UPROPERTY(VisibleDefaultsOnly, Category = "Stat")
		float Point;

	UPROPERTY(EditDefaultsOnly, Category = "Animation | Dive")
		float MaxDiveTime;
	UPROPERTY(VisibleAnywhere, Category = "Animation | Tumble")
		float CurrentTumbledTime;
	UPROPERTY(EditDefaultsOnly, Category = "Animation | Tumble")
		float MaxTumbledTime;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "잡기 최대 유지시간"), Category = "Animation | Grab")
		float MaxGrabTime;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "잡기 재사용시간"), Category = "Animation | Grab")
		float GrabCoolTime;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "잡기당할 때 이동속도  "), Category = "Animation | Grab")
		float GrabbedSpeed;
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "잡기당할 때 회전 속도"), Category = "Animation | Grab")
		FRotator GrabbedRotationSpeed;

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

	UPROPERTY(EditDefaultsOnly, Category = "CharacterSpeed")
		float OriginalSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "CharacterSpeed")
		FRotator OriginalRotationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Interpolation")
		float InterTime;
	UPROPERTY(VisibleAnywhere, Category = "Interpolation")
		float CurrentInterTime;
	UPROPERTY(EditDefaultsOnly, Category = "Interpolation")
		float StopTime;
	UPROPERTY(VisibleAnywhere, Category = "Interpolation")
		float CurrentStopTime;
	UPROPERTY(VisibleAnywhere, Category = "Interpolation")
		FVector PreviousLocation;
	UPROPERTY(VisibleAnywhere, Category = "Interpolation")
		FVector PreviousVelocity;
	UPROPERTY(VisibleAnywhere, Category = "Interpolation")
		FVector InterVelocity;

	UPROPERTY(VisibleAnywhere, Category = "Enums")
		EMovementState MovementState;
	UPROPERTY(VisibleAnywhere, Category = "Enums")
		EDepartment Department;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,meta=(AllowPrivateAccess = true), Category = "Texture")
		TMap<EDepartment, UTexture*> DepartmentTextureMap;

	UPROPERTY(VisibleAnywhere, Category = "Animation | Grab")
		ATinoCharacter* Target;

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

	FTimerHandle DiveTimer;
	FTimerHandle GrabTimer;
	FTimerHandle GrabCoolTimer;

};
