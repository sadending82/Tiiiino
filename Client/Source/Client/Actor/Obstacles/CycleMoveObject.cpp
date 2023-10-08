// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Obstacles/CycleMoveObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
ACycleMoveObject::ACycleMoveObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject <USceneComponent>("RootSceneComponent");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>("Direction");


	SceneRootComponent->SetupAttachment(RootComponent);
	Mesh->SetupAttachment(SceneRootComponent);
	DirectionArrow->SetupAttachment(SceneRootComponent);
}

// Called when the game starts or when spawned
void ACycleMoveObject::BeginPlay()
{
	Super::BeginPlay();
	
	StartPos = GetActorLocation();
}

// Called every frame
void ACycleMoveObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsStartMove)
	{
		CycleDeltaTime += DeltaTime;
		
		SetActorLocation(StartPos + DirectionArrow->GetForwardVector() * sin(CycleDeltaTime * CyclePerSeconds * PI * 2) * MoveDistance);
	}

}

