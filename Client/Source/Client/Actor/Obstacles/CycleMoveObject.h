// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "CycleMoveObject.generated.h"

UCLASS()
class CLIENT_API ACycleMoveObject : public ABaseObstacle
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACycleMoveObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere) class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere) class UArrowComponent* DirectionArrow;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float CycleDeltaTime = 0.f;

	// 기준이 되는 시작점을 저장하기 위한 변수
	FVector StartPos;

	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float CyclePerSeconds = 1.f;

	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float MoveDistance = 500.f;
};
