// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Obstacles/RotatingPole.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Global.h"

// Sets default values
ARotatingPole::ARotatingPole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Rotation_Pole"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ARotatingPole::BeginPlay()
{
	Super::BeginPlay();
}

void ARotatingPole::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* hitCharacter = Cast<ACharacter>(Other);

	if (hitCharacter != nullptr)
	{		
		CLog::Log("Hit");
		if (DoElasticCollision)
		{
			hitCharacter->LaunchCharacter(FVector(-HitNormal.X, -HitNormal.Y, fabs(HitNormal.Z)) * ElasticForce, false, false);
		}
	}
}


// Called every frame
void ARotatingPole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsStartMove)
	{
		// 일단은 임시적으로 RotateSpeed를 설정하여 계속해서 돌기만 하도록 설정한다.
		// 동기화를 하는 경우는 SetActorRotation을 통해 직접 각도를 계산하여 넣어야 할 것으로 보인다.
		// FRotator는 pitch, yaw, roll의 순서임을 까먹으면 안됨.
		AddActorLocalRotation(FRotator(0.0f, RotateSpeed * DeltaTime, 0.0f));
	}
}

