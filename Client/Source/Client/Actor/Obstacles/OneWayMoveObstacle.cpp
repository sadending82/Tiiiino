// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Obstacles/OneWayMoveObstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Global.h"

AOneWayMoveObstacle::AOneWayMoveObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject <USceneComponent>("RootSceneComponent");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>("Direction");
	

	SceneRootComponent->SetupAttachment(RootComponent);
	Mesh->SetupAttachment(SceneRootComponent);
	DirectionArrow->SetupAttachment(SceneRootComponent);
}

void AOneWayMoveObstacle::BeginPlay()
{
	Super::BeginPlay();

	// 임시적으로 추가하였음
	// 실제로는 네트워크에서 패킷을 보내 주었을 때에 변경해주어야 함
	bIsStartMove = true;
}

void AOneWayMoveObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsStartMove)
	{
		AddActorWorldOffset(DirectionArrow->GetForwardVector() * MoveSpeed * DeltaTime);
	}
}
