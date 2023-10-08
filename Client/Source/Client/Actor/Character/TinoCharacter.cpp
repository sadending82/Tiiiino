#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"
#include "Global.h"

#include "Network/Network.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actor/Character/CharacterAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "MenuUI/InGameUIWidget.h"
#include "MenuUI/DialogUIWidget.h"

ATinoCharacter::ATinoCharacter()
	:MaxTumbledTime(1.0f),
	MaxGrabTime(3.0f),
	GrabCoolTime(1.0f),
	GrabbedSpeed(100.f),
	GrabbedRotationSpeed(FRotator(0.f,0.f,108.f)),
	DetectDist(100.f),
	DetectRadius(50.f),
	DetectAngle(60.f),
	TargetInterval(50.f),
	OriginalSpeed(400.f),
	MovementState(EMovementState::EMS_Normal),
	Target(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	UHelpers::CreateComponent<USpringArmComponent>(this, &SpringArm, "SpringArm", GetCapsuleComponent());
	UHelpers::CreateComponent<UCameraComponent>(this, &Camera, "Camera", SpringArm);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = true;
	
	OriginalRotationSpeed = GetCharacterMovement()->RotationRate;
	
}

void ATinoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATinoCharacter::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATinoCharacter::OnMoveRight);
	PlayerInputComponent->BindAxis("HorizonLock", this, &ATinoCharacter::OnHorizonLock);
	PlayerInputComponent->BindAxis("VerticalLock", this, &ATinoCharacter::OnVerticalLock);

	PlayerInputComponent->BindAction("CreateDummy", EInputEvent::IE_Pressed, this, &ATinoCharacter::CreateDummy);
	PlayerInputComponent->BindAction("Align", EInputEvent::IE_Pressed, this, &ATinoCharacter::Align);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ATinoCharacter::Jump);
	PlayerInputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &ATinoCharacter::OnGrab);
	PlayerInputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &ATinoCharacter::OffGrab);
	//PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ATinoCharacter::StopJumping);
	PlayerInputComponent->BindAction("Dive", EInputEvent::IE_Pressed, this, &ATinoCharacter::Dive);
}

// Called when the game starts or when spawned
void ATinoCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!!GetController())
	{
		if (GetController()->IsPlayerController())
		{
			//카메라 각도 제한(마우스 Y축 아래로 제한)
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 0.f;
		}
		else
		{
			GetCharacterMovement()->BrakingDecelerationWalking = 0;
			GetCharacterMovement()->BrakingFrictionFactor = 0;
			GetCharacterMovement()->GravityScale = 0.0;
		}
	}

}

void ATinoCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	if (bIsControlledPlayer)
	{
		Network::GetNetwork()->release();
	}

}

// Called every frame
void ATinoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		float PitchClamp = FMath::ClampAngle(Rotation.Pitch, -45.f, 30.f);
		FRotator RotationControl(PitchClamp, Rotation.Yaw, Rotation.Roll);
		SleepEx(0, true);

		if (Network::GetNetwork()->bIsConnected)
		{
			if (GetController())
			{
				if (!GetController()->IsPlayerController())
				{
					//서버랑 연결 돼 있을 때만 상대 캐릭터 보간하려
					//Update GroundSpeedd (22-04-05)
					//GroundSpeedd = ServerStoreGroundSpeed;
					//Update Interpolation (23-09-27)
					GetCharacterMovement()->Velocity = ServerCharMovingSpeed;

				}
				else {
					if (Network::GetNetwork()->bGameIsStart)
					{
						auto pos = GetTransform().GetLocation();
						auto rot = GetTransform().GetRotation();

						ServerSyncElapsedTime += DeltaTime;
						if (ServerSyncDeltaTime < ServerSyncElapsedTime)
						{
							send_move_packet(Network::GetNetwork()->s_socket, Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->bIsAir, pos.X, pos.Y, pos.Z, rot, GetVelocity().Size2D(), GetCharacterMovement()->Velocity);
							ServerSyncElapsedTime = 0.0f;
						}

						float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
					}

				}
			}
			
		}

		PlayTumbleMontage(DeltaTime);
		
		if (MovementState == EMovementState::EMS_Grabbing && Target != nullptr)
		{
			SetActorLocation(Target->GetActorLocation() - GetActorForwardVector() * TargetInterval);
			//SetActorRotation(Target->GetActorRotation());
		}

		// 10/04 가만히 있을 때 충돌하지 안흔 부분을 해결하기 위한 코드 추가
		FHitResult OutHit;
		GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.01f), GetActorRotation(), true, OutHit);
		GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.01f), GetActorRotation(), true, OutHit);

		if (bIsSpactateModeEnabled)
		{
			if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying)
			{
				GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
			}
		}

	}
}
bool ATinoCharacter::CanTumble(float DeltaTime)
{
	bool ret = true;

	ret &= GetCharacterMovement()->IsFalling();
	//ret &= (GetVelocity().Z != FMath::IsNearlyZero());

	if (ret && MaxTumbledTime > CurrentTumbledTime) CurrentTumbledTime += DeltaTime;
	bCanTumbled = (CurrentTumbledTime >= MaxTumbledTime);

	return ret;
}


void ATinoCharacter::PlayTumbleMontage(float DeltaTime)
{
	CanTumble(DeltaTime);

	if (bCanTumbled && !GetCharacterMovement()->IsFalling())
	{
		if (TumbleMontage)
		{
			CurrentTumbledTime = 0.f;
			bCanTumbled = false;
			SendAnimPacket(3);
			PlayAnimMontage(TumbleMontage);
		}
		else
			CLog::Log("Asset TumbleMontage is Invalid");
	}
	else if (!bCanTumbled && !GetCharacterMovement()->IsFalling())
	{
		CurrentTumbledTime = 0.f;
	}
}

void ATinoCharacter::Align()
{
	GetController()->SetControlRotation(GetActorForwardVector().Rotation());
}

void ATinoCharacter::MakeAndShowHUD()
{
	InGameWidgetInstance = CreateWidget<UInGameUIWidget>(GetWorld(), InGameWidgetClass);
	InGameWidgetInstance->AddToViewport();

}

void ATinoCharacter::MakeAndShowDialog()
{
	DialogWidget = CreateWidget<UDialogUIWidget>(GetWorld(), DialogWidgetClass);
	DialogWidget->AddToViewport();
}


bool ATinoCharacter::SendAnimPacket(int32 AnimType)
{
	if (!!GetController())
	{
		if (GetController()->IsPlayerController())
		{
			send_action_packet(Network::GetNetwork()->s_socket, static_cast<int>(AnimType));
			return true;
		}
	}
	else
	{
		CLog::Log("Controller is nullptr");
	}
	return false;

}

void ATinoCharacter::DiveBegin()
{
}

void ATinoCharacter::DiveEnd()
{
	bIsDiving = false;
}

void ATinoCharacter::OnAccelEffect()
{
	//비네트 값을 조절해 가속 이펙트를 킴
	Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
	Camera->PostProcessSettings.VignetteIntensity = CustomVignetteIntensity;
}

void ATinoCharacter::OffAccelEffect()
{
	Camera->PostProcessSettings.bOverride_VignetteIntensity = false;

}

void ATinoCharacter::SetOriginalSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
}

void ATinoCharacter::TimerStart()
{
	GetWorldTimerManager().SetTimer(InGameUITimerHandle, this, &ATinoCharacter::TimerEnd, 1.f, true);
}

void ATinoCharacter::TimerEnd()
{

	// 유효성 확인
	if (InGameWidgetInstance)
	{
		// UInGameUIWidget의 TimerRun 함수 호출
		InGameWidgetInstance->TimerRun();
	}
	if (InGameWidgetInstance->GetRestGameTime() < 0)
	{
		InGameWidgetInstance->TimerEnd();
		GetWorldTimerManager().ClearTimer(InGameUITimerHandle);
	}

}

void ATinoCharacter::Dive()
{
	if (DiveMontage && CanDive())
	{
		bIsDiving = true;
		SendAnimPacket(2);
		PlayAnimMontage(DiveMontage);
	}
}

bool ATinoCharacter::GetIsAirForNetwork()
{
	auto AnimInstance = GetTinoAnimInstance();
	if (!!AnimInstance)
	{
		return AnimInstance->bIsAirForNetwork;
	}
	return false;
}

void ATinoCharacter::SetIsAirForNetwork(bool val)
{
	auto AnimInstance = GetTinoAnimInstance();
	if (!!AnimInstance)
	{
		AnimInstance->bIsAirForNetwork = val;
	}
}

UCharacterAnimInstance* ATinoCharacter::GetTinoAnimInstance()
{
	return Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void ATinoCharacter::OnMoveForward(float Axis)
{
	if (CanMove())
	{
		FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
		FVector dir = FQuat(rotator).GetForwardVector().GetUnsafeNormal2D();

		AddMovementInput(dir, Axis);
	}

}

void ATinoCharacter::OnMoveRight(float Axis)
{
	if (CanMove())
	{
		FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
		FVector dir = FQuat(rotator).GetRightVector().GetUnsafeNormal2D();

		AddMovementInput(dir, Axis);
	}

}

void ATinoCharacter::OnHorizonLock(float Axis)
{
	AddControllerYawInput(Axis);
}

void ATinoCharacter::OnVerticalLock(float Axis)
{
	AddControllerPitchInput(Axis);
}

void ATinoCharacter::CreateDummy()
{
	GetController<ATinoController>()->CreateDummy();
}

void ATinoCharacter::Jump()
{
	if (CanMove() && GetCharacterMovement()->IsFalling() == false)
	{
		Super::Jump();
		SendAnimPacket(1);
	}

	if (GetController())
	{
		if (!GetController()->IsPlayerController())
		{
			Super::Jump();
		}
	}
}

void ATinoCharacter::StopJumping()
{
	Super::StopJumping();
	SetMovementState(EMovementState::EMS_Normal);
}

void ATinoCharacter::PlayTumbleMontage()
{
	PlayAnimMontage(TumbleMontage);
}

void ATinoCharacter::OnGrab()
{
	if (GrabMontage == nullptr || (CanGrab() == false)) return;

	bIsGrabbing = true;

	SendAnimPacket(4);
	PlayAnimMontage(GrabMontage);
	//if (GetController() && GetController()->IsPlayerController())
		//CLog::Print("GrabOn");
	

}

void ATinoCharacter::OffGrab()
{
	SendAnimPacket(5);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	StopAnimMontage(GrabMontage);
	if (GetController() && GetController()->IsPlayerController())
		SetMovementState(EMovementState::EMS_Normal);

	SetGrabbedToNormal();


	bIsGrabbing = false;
	bIsGrabCoolTime = true;

	if (GetWorldTimerManager().IsTimerActive(GrabCoolTimer) == false)
	{	//그랩 쿨타임 시작
		GetWorldTimerManager().SetTimer(GrabCoolTimer, [this]()
			{
				bIsGrabCoolTime = false;
			}
		, GrabCoolTime, false);
	}

	//if (GetController() && GetController()->IsPlayerController())
		//CLog::Print("GrabOff");


}

void ATinoCharacter::SetNormalToGrabbed()
{
	if (!!Target)
	{
		auto Other = Cast<ATinoCharacter>(Target);
		Other->SetMovementState(EMovementState::EMS_IsGrabbed);
		Other->GetCharacterMovement()->MaxWalkSpeed = GrabbedSpeed;
		Other->GetCharacterMovement()->RotationRate = GrabbedRotationSpeed;
	}
	else
		CLog::Log("Target is Nullptr");
}

void ATinoCharacter::SetGrabbedToNormal()
{
	if (!!Target)
	{
		//if(GetController() && GetController()->IsPlayerController())
			//CLog::Print("Target Remove");
		auto Other = Cast<ATinoCharacter>(Target);
		Other->SetMovementState(EMovementState::EMS_Normal);
		Other->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
		Other->GetCharacterMovement()->RotationRate = OriginalRotationSpeed;
		Target = nullptr;
	}
	else
		CLog::Log("No targets to delete");
}

void ATinoCharacter::GrabBegin()
{
	//그랩 쿨타임 시작
	//if (GetController() && GetController()->IsPlayerController())
		//CLog::Print("Grab Timer ON");
	GetWorldTimerManager().SetTimer(GrabTimer, this, &ATinoCharacter::OffGrab, MaxGrabTime, false);
}


void ATinoCharacter::DetectTarget()
{
	FVector start = GetActorLocation();
	FVector end = start + GetActorForwardVector() * DetectDist;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	EObjectTypeQuery Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	TArray<AActor*> IgnoreActors;
	//자기자신은 충돌검사 X
	IgnoreActors.Add(this);

	FHitResult HitResult;
	bool result = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),
		start, end, DetectRadius,
		ObjectTypes,
		false,
		IgnoreActors,
		bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true);

	if (result && bIsGrabCoolTime == false)
	{
		Target = HitResult.GetActor();
		//float ScalarValue = GetActorForwardVector().Dot(Target->GetActorForwardVector());
		
		// 이미 잡힌 캐릭터나 잡고있는 캐릭터는 잡기 대상이 될 수 없다(기차 방지)
		EMovementState TargetState = Cast<ATinoCharacter>(Target)->GetMovementState();
		switch (TargetState)
		{
		case EMovementState::EMS_Grabbing :
		case EMovementState::EMS_IsGrabbed :
			Target = nullptr;
			return;
		default:
			break;
		}

		FVector DirVec = Target->GetActorLocation() - GetActorLocation();
		double angle = FMath::Acos(static_cast<double>(GetActorForwardVector().Dot(DirVec.GetUnsafeNormal())));
		angle = FMath::RadiansToDegrees(angle);
		if (angle < DetectAngle / 2.f)
		{
			SetMovementState(EMovementState::EMS_Grabbing);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetNormalToGrabbed();
		}
		else
		{
			CLog::Log("Target is bigger than DetectAngle, Go to Location In DectectAngle");
			Target = nullptr;
		}
		//앞뒤 판정
		/*if (ScalarValue > 0)
		{
			
		}
		else
		{
			CLog::Log("Target is front, Go to the Target's Back");
			Target = nullptr;
		}*/
	}
}

void ATinoCharacter::DisableInputMode()
{
	DisableInput(GetController<APlayerController>());
}

void ATinoCharacter::EnableInputMode()
{
	EnableInput(GetController<APlayerController>());
}

bool ATinoCharacter::CanMove()
{
	bool ret = true;

	switch (MovementState)
	{
	case EMovementState::EMS_Normal:
	case EMovementState::EMS_Fall:
	case EMovementState::EMS_IsGrabbed:
		ret = true;
		break;
	default:
		ret = false;
	}
	return ret;
}

bool ATinoCharacter::CanDive()
{
	bool ret = false;

	switch (MovementState)
	{
	case EMovementState::EMS_Normal:
	case EMovementState::EMS_Fall:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}
	ret &= (bIsDiving == false);
	return ret;
}

bool ATinoCharacter::CanGrab()
{
	bool ret = false;

	if (bIsGrabCoolTime == true)
		ret = false;

	switch (MovementState)
	{
	case EMovementState::EMS_Normal:
	case EMovementState::EMS_Fall:
	case EMovementState::EMS_Grabbing:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}


	return ret;
}


