// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Obstacles/RotatingGround.h"

// Sets default values
ARotatingGround::ARotatingGround()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Rotation_Pole"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ARotatingGround::BeginPlay()
{
	Super::BeginPlay();
	
	// �ӽ������� true�� �����Ͽ���
	// �����δ� ����ȭ ��Ŷ�� �޾��� ���� ������ �־�� ��.
	bIsStartMove = true;
}

void ARotatingGround::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{

}

// Called every frame
void ARotatingGround::Tick(float DeltaTime)
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

