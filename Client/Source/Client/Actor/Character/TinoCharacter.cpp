#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"
#include "Global.h"

#include "Network/Network.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"

ATinoCharacter::ATinoCharacter()
	:MaxTumbledTime(0.5f)
{
	PrimaryActorTick.bCanEverTick = true;
	UHelpers::CreateComponent<USpringArmComponent>(this, &SpringArm, "SpringArm",GetCapsuleComponent());
	UHelpers::CreateComponent<UCameraComponent>(this, &Camera, "Camera", SpringArm);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = true;
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
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ATinoCharacter::StopJumping);
	PlayerInputComponent->BindAction("Dive", EInputEvent::IE_Pressed, this, &ATinoCharacter::Dive);
	PlayerInputComponent->BindAction("Running", EInputEvent::IE_Pressed, this, &ATinoCharacter::OnRunning);
	PlayerInputComponent->BindAction("Running", EInputEvent::IE_Released, this, &ATinoCharacter::OffRunning);
}

// Called when the game starts or when spawned
void ATinoCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATinoCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	Network::GetNetwork()->release();
}

// Called every frame
void ATinoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayTumbleMontage(DeltaTime);
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		float PitchClamp = FMath::ClampAngle(Rotation.Pitch, -45.f, 30.f);
		FRotator RotationControl(PitchClamp, Rotation.Yaw, Rotation.Roll);

		if (GetController()->IsPlayerController() && Network::GetNetwork()->bIsConnected) {
			SleepEx(0, true);
			auto pos = GetTransform().GetLocation();
			auto rot = GetTransform().GetRotation();

			ServerSyncElapsedTime += DeltaTime;
			if (ServerSyncDeltaTime < ServerSyncElapsedTime)
			{
				send_move_packet(Network::GetNetwork()->s_socket,GetCharacterMovement()->IsFalling(), pos.X, pos.Y, pos.Z, rot, GetVelocity().Size2D(), GetCharacterMovement()->Velocity);
				ServerSyncElapsedTime = 0.0f;
			}

			float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();

			
		}
		else {
			//Update GroundSpeedd (22-04-05)
			//GroundSpeedd = ServerStoreGroundSpeed;
			//Update Interpolation (22-11-25)
			//GetCharacterMovement()->Velocity = CharMovingSpeed;
		}

	}
}
bool ATinoCharacter::CanTumble(float DeltaTime)
{
	bool ret = true;

	ret &= GetCharacterMovement()->IsFalling();
	ret &= (GetVelocity().Z < 0);
	
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

void ATinoCharacter::Dive()
{
	if(DiveMontage)
		PlayAnimMontage(DiveMontage);
}

void ATinoCharacter::OnMoveForward(float Axis)
{
	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	FVector dir = FQuat(rotator).GetForwardVector().GetUnsafeNormal2D();

	AddMovementInput(dir, Axis);
}

void ATinoCharacter::OnMoveRight(float Axis)
{
	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	FVector dir = FQuat(rotator).GetRightVector().GetUnsafeNormal2D();

	AddMovementInput(dir, Axis);
}

void ATinoCharacter::OnHorizonLock(float Axis)
{
	AddControllerYawInput(Axis);
}

void ATinoCharacter::OnVerticalLock(float Axis)
{
	AddControllerPitchInput(Axis);
}

void ATinoCharacter::OnRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = 200;
}

void ATinoCharacter::OffRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = 100;
}

void ATinoCharacter::CreateDummy()
{
	GetController<ATinoController>()->CreateDummy();
}

void ATinoCharacter::Jump()
{
	Super::Jump();
	
}

void ATinoCharacter::StopJumping()
{
	Super::StopJumping();
}
