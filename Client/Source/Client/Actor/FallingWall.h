// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Obstacles/BaseObstacle.h"
#include "FallingWall.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API AFallingWall : public ABaseObstacle
{
	GENERATED_BODY()
	
public:
	AFallingWall();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void MeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class USceneComponent* SceneComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UStaticMeshComponent* LeftMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UStaticMeshComponent* RightMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UGeometryCollectionComponent* LeftBreakableMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UGeometryCollectionComponent* RightBreakableMesh;

	UPROPERTY(EditAnywhere, Category = "Stat")
		bool bIsFallable;
};
