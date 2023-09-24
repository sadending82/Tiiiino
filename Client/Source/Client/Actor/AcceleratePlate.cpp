// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AcceleratePlate.h"
#include "Global.h"
#include "GameFramework/Character.h"


// Sets default values
AAcceleratePlate::AAcceleratePlate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneRootComponent = CreateDefaultSubobject <USceneComponent>("RootSceneComponent");
	OverlapMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("AcceleratePlateMesh"));
	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>("AcceleratePlateMainMesh");
	

	SceneRootComponent->SetupAttachment(RootComponent);
	OverlapMesh->SetupAttachment(SceneRootComponent);
	MainMesh->SetupAttachment(SceneRootComponent);

	OverlapMesh->OnComponentBeginOverlap.AddDynamic(this, &AAcceleratePlate::OnOverlapBegin);
	OverlapMesh->OnComponentEndOverlap.AddDynamic(this, &AAcceleratePlate::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AAcceleratePlate::BeginPlay()
{
	Super::BeginPlay();

}

void AAcceleratePlate::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ACharacter* MyCharacter = Cast<ACharacter>(OtherActor);

	CLog::Log("Overlapped!");

	if (MyCharacter != nullptr)
	{
		CLog::Log("CASTING_SUCCESS");
	}
}

void AAcceleratePlate::NotifyActorEndOverlap(AActor* OtherActor)
{
}

void AAcceleratePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = Cast<ACharacter>(OtherActor);
	if (MyCharacter != nullptr)
	{
		CLog::Log("CASTING_SUCCESS_OOB");
	}
}

void AAcceleratePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AAcceleratePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

