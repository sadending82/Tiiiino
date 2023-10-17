// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Obstacles/BaseObstacle.h"
#include "XYFan.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API AXYFan : public ABaseObstacle
{
	GENERATED_BODY()

public:
	AXYFan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class USceneComponent* SceneComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UStaticMeshComponent* FanMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UStaticMeshComponent* CoverMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UBoxComponent* OverlapBox;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UArrowComponent* DirectionArrow;

	UPROPERTY(EditAnywhere, Category = "Stat")
		float FanPower;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float FanRotateSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
		TSubclassOf<class ATinoCharacter> OverlapFilter;
	UPROPERTY(VisibleAnywhere, Category = "Stat")
		TSet<AActor*> OverlappingActors;
};
