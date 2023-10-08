// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/Obstacles/BaseObstacle.h"

// Sets default values
ABaseObstacle::ABaseObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABaseObstacle::BeginPlay()
{
	Super::BeginPlay();

	// 10/08 김혁동
	// 그냥 부모에서 적용하면 되는 걸 각각의 자식에서 하고 있었으므로
	// 부모에서 적용하는 것으로 변경하였음.
	bIsStartMove = true;
	
}

// Called every frame
void ABaseObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

