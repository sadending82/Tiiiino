#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"
#include "Global.h"

#include "Interface/Network.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"

ATinoCharacter::ATinoCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ATinoCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ATinoCharacter::StopJumping);
	PlayerInputComponent->BindAction("Running", EInputEvent::IE_Pressed, this, &ATinoCharacter::OnRunning);
	PlayerInputComponent->BindAction("Running", EInputEvent::IE_Released, this, &ATinoCharacter::OffRunning);
}

// Called when the game starts or when spawned
void ATinoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetController()->IsPlayerController())
	{
		Network::GetNetwork()->mMyCharacter = this;

		if (ConnectServer())
			send_login_packet(s_socket);
	}
}

void ATinoCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	closesocket(s_socket);
	Network::GetNetwork()->release();
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

		if (GetController()->IsPlayerController()) {
			SleepEx(0, true);
			auto pos = GetTransform().GetLocation();
			auto rot = GetTransform().GetRotation();

			ServerSyncElapsedTime += DeltaTime;
			if (ServerSyncDeltaTime < ServerSyncElapsedTime)
			{
				send_move_packet(s_socket, GetCharacterMovement()->IsFalling(), pos.X, pos.Y, pos.Z, rot, GetVelocity().Size2D(), GetCharacterMovement()->Velocity);
				ServerSyncElapsedTime = 0.0f;
			}

			float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
			//GroundSpeedd = CharXYVelocity;

			
		}
		else {
			//Update GroundSpeedd (22-04-05)
			//GroundSpeedd = ServerStoreGroundSpeed;
			//Update Interpolation (22-11-25)
			//GetCharacterMovement()->Velocity = CharMovingSpeed;
		}

		//auto a = GetTransform().GetLocation().Y;
		//Controller->SetControlRotation(RotationControl);
	}
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



void ATinoCharacter::RecvPacket()
{
	Super::RecvPacket();
	DWORD recv_flag = 0;
	ZeroMemory(&recv_expover.GetWsaOver(), sizeof(recv_expover.GetWsaOver()));
	
	recv_expover.GetWsaBuf().buf = reinterpret_cast<char*>(recv_expover.GetBuf() + _prev_size);
	recv_expover.GetWsaBuf().len = BUF_SIZE - _prev_size;
	
	int ret = WSARecv(s_socket, &recv_expover.GetWsaBuf(), 1, NULL, &recv_flag, &recv_expover.GetWsaOver(), recv_Gamecallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
		}
	}
}


bool ATinoCharacter::ConnectServer()
{
	Super::ConnectServer();
	if (bIsConnected) return false;

	s_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);

	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	int rt = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (SOCKET_ERROR == rt)
	{
		std::cout << "connet Error :";
		int err_num = WSAGetLastError();
		//error_display(err_num);
		//system("pause");
		//UE_LOG(LogTemp, Error, TEXT("Conn Error %d"), err_num);
		//exit(0);
		closesocket(s_socket);
		return false;
	}


	DWORD recv_flag = 0;
	int ret = WSARecv(s_socket, &recv_expover.GetWsaBuf(), 1, NULL, &recv_flag, &recv_expover.GetWsaOver(), recv_Gamecallback);
	if (SOCKET_ERROR == ret)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			//error ! 
			return false;
		}
	}
	return true;
}
