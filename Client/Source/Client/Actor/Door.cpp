// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Door.h"
#include "Global.h"
// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject <USceneComponent>("RootSceneComponent");
	LeftMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeftDoor");
	RightMesh = CreateDefaultSubobject<UStaticMeshComponent>("RightDoor");

	SceneRootComponent->SetupAttachment(RootComponent);
	LeftMesh->SetupAttachment(SceneRootComponent);
	RightMesh->SetupAttachment(SceneRootComponent);

}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	fCalcTime += DeltaTime;

	if (fCalcTime > DoorOpenTerm)
	{
		fCalcTime -= DoorOpenTerm;

		switch (OpenType)
		{
		case WaitToOpen:
			SetStartOpenDoor();
			OpenType = OpenToWait;
			break;
		case WaitToClose:
			SetStartCloseDoor();
			OpenType = CloseToWait;
			break;
		case OpenToWait:
		case CloseToWait:
		default:
			break;
		}
	}

	if (bIsOpeningDoor) {
		if (fRemainMoveDist > DoorMoveSpeed * DeltaTime)
		{
			LeftMesh->AddRelativeLocation(FVector(0.f, DoorMoveSpeed * DeltaTime, 0.f));
			RightMesh->AddRelativeLocation(FVector(0.f, -DoorMoveSpeed * DeltaTime, 0.f));
			fRemainMoveDist -= DoorMoveSpeed * DeltaTime;
		}
		else
		{
			LeftMesh->AddRelativeLocation(FVector(0.f, fRemainMoveDist, 0.f));
			RightMesh->AddRelativeLocation(FVector(0.f, -fRemainMoveDist, 0.f));
			fRemainMoveDist = 0.f;
			bIsOpeningDoor = false;
			OpenType = WaitToClose;
		}
	}

	if (bIsClosingDoor) {
		if (fRemainMoveDist > DoorMoveSpeed * DeltaTime)
		{
			LeftMesh->AddRelativeLocation(FVector(0.f, -DoorMoveSpeed * DeltaTime, 0.f));
			RightMesh->AddRelativeLocation(FVector(0.f, DoorMoveSpeed * DeltaTime, 0.f));
			fRemainMoveDist -= DoorMoveSpeed * DeltaTime;
		}
		else
		{
			LeftMesh->AddRelativeLocation(FVector(0.f, -fRemainMoveDist, 0.f));
			RightMesh->AddRelativeLocation(FVector(0.f, fRemainMoveDist, 0.f));
			fRemainMoveDist = 0.f;
			bIsClosingDoor = false;
			OpenType = WaitToOpen;
		}
		
	}
	
}

void ADoor::SetStartOpenDoor()
{
	bIsOpeningDoor = true;
	fRemainMoveDist = DoorOpenDistance;
}

void ADoor::SetStartCloseDoor()
{
	bIsClosingDoor = true;
	fRemainMoveDist = DoorOpenDistance;
}

void ADoor::SetBaseTime(float value)
{
	fBaseTime = value;
	fCalcTime = value;
}

