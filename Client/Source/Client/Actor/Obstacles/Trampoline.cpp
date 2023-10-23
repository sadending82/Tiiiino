// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Obstacles/Trampoline.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshcomponent.h"
#include "Components/Capsulecomponent.h"

// Sets default values
ATrampoline::ATrampoline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRootComponent = CreateDefaultSubobject <USceneComponent>("RootSceneComponent");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrampolineMesh"));
	JumpingMesh = CreateDefaultSubobject<USkeletalMeshComponent>("jumper");
	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>("CollisionMesh");

	SceneRootComponent->SetupAttachment(RootComponent);
	Mesh->SetupAttachment(SceneRootComponent);
	JumpingMesh->SetupAttachment(SceneRootComponent);
	CollisionMesh->SetupAttachment(SceneRootComponent);

}

// Called when the game starts or when spawned
void ATrampoline::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentHit.AddDynamic(this, &ATrampoline::OnHit);
}

void ATrampoline::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* hitCharacter = Cast<ACharacter>(OtherActor);
	CLog::Log("Trampoline Hit");
	if (hitCharacter != nullptr)
	{
		hitCharacter->LaunchCharacter(FVector(0.f, 0.f, ElasticForce), false, false);
		JumpingMesh->PlayAnimation(HitAnimation, false);
		ASoundManager::GetSoundManager()->PlaySFXAtLocation(this,ESFXType::ESFXType_Trapoline, GetActorLocation());
	}
}

// Called every frame
void ATrampoline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

