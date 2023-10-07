// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Obstacles/Trampoline.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATrampoline::ATrampoline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrampolineMesh"));
	RootComponent = Mesh;
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &JumpingMesh, "TramPolineJumpMesh", Mesh);

	
}

// Called when the game starts or when spawned
void ATrampoline::BeginPlay()
{
	Super::BeginPlay();
	JumpingMesh->OnComponentHit.AddDynamic(this, &ATrampoline::OnHit);
}

void ATrampoline::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* hitCharacter = Cast<ACharacter>(OtherActor);
	CLog::Log("Trampoline Hit");
	if (hitCharacter != nullptr)
	{
		hitCharacter->LaunchCharacter(FVector(0.f, 0.f, ElasticForce), false, false);
	}
}

// Called every frame
void ATrampoline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

