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
	
	// �ӽ������� true�� �����Ͽ���
	// �����δ� ����ȭ ��Ŷ�� �������� ���� ������ �־�� ��.
	bIsStartMove = true;
}

void ARotatingPole::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* hitCharacter = Cast<ACharacter>(Other);

	if (hitCharacter != nullptr)
	{
		//Ȥ�� ������ �߻��ؼ� �����ΰǰ�?
		
		//hitCharacter->LaunchCharacter(-HitNormal * ElasticForce, false, false);
		
		CLog::Log("Hit");

		hitCharacter->LaunchCharacter(FVector(-HitNormal.X, -HitNormal.Y, fabs(HitNormal.Z))*ElasticForce, false, false);
		
	}
}


// Called every frame
void ARotatingPole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsStartMove)
	{
		// �ϴ��� �ӽ������� RotateSpeed�� �����Ͽ� ����ؼ� ���⸸ �ϵ��� �����Ѵ�.
		// ����ȭ�� �ϴ� ���� SetActorRotation�� ���� ���� ������ ����Ͽ� �־�� �� ������ ���δ�.
		// FRotator�� pitch, yaw, roll�� �������� ������� �ȵ�.
		AddActorLocalRotation(FRotator(0.0f, RotateSpeed * DeltaTime, 0.0f));
	}
}

