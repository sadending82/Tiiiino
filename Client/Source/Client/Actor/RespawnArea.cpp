// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RespawnArea.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARespawnArea::ARespawnArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UHelpers::CreateComponent<UBoxComponent>(this, &OverlapMesh, "RespawnTriggerBox");

	RootComponent = OverlapMesh;
}

// Called when the game starts or when spawned
void ARespawnArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARespawnArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ACharacter* MyCharacter = Cast<ACharacter>(OtherActor);
	if (MyCharacter != nullptr)
	{
		MyCharacter->SetActorLocation(RespawnPoint, false, (FHitResult*)nullptr, ETeleportType::ResetPhysics);
		MyCharacter->GetCharacterMovement()->StopMovementImmediately();
	}
}

// Called every frame
void ARespawnArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

