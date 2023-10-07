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

	// �ӽ������� �߰��Ͽ���
	// �����δ� ��Ʈ��ũ���� ��Ŷ�� ���� �־��� ���� �������־�� ��
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
