// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Obstacles/BaseObstacle.h"
#include "OneWayMoveObstacle.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API AOneWayMoveObstacle : public ABaseObstacle
{
	GENERATED_BODY()
	
public:
	AOneWayMoveObstacle();

protected:
	virtual void BeginPlay() override;

	class USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere) class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere) class UArrowComponent* DirectionArrow;
 
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float MoveSpeed = 50.f;
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float MoveDistance = 1000.f;
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) FVector OriginalLocation;
	
};
