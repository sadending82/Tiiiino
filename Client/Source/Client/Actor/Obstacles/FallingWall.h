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

	UFUNCTION()
	void MeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:

	UFUNCTION()
	virtual	void ActionObject() override;
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

	UPROPERTY(VisibleDefaultsOnly, Category = "Field")
		class UFieldSystemComponent* FieldSystemComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Field")
		class URadialFalloff* RadialFalloff;
	UPROPERTY(VisibleDefaultsOnly, Category = "Field")
		class URadialVector* RadialVector;
	UPROPERTY(VisibleDefaultsOnly, Category = "Field")
		class UFieldSystemMetaDataFilter* MetaData;

	UPROPERTY(EditAnywhere, meta=(ToolTip="퍼지는 힘?"), Category = "Stat")
		float RadialFalloffMagnitude;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "미는 힘?"), Category = "Stat")
		float RadialVectorMagnitude;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "True 설정시 파괴됨"), Category = "Stat")
		bool bIsBreakable;

	UPROPERTY(EditAnywhere, meta=(MakeEditWidget ,ToolTip="티노와의 접촉지점"), Category = "Stat")
		FVector FieldLocation;
};
