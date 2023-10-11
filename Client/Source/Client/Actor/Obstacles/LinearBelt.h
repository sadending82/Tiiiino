// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "LinearBelt.generated.h"

UCLASS()
class CLIENT_API ALinearBelt : public ABaseObstacle
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALinearBelt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class USceneComponent* SceneComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UBoxComponent* OverlapBox;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UArrowComponent* DirectionArrow;

	UPROPERTY(EditAnywhere, Category = "Stat")
		float MaterialSpeedY;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float BeltSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Stat")
		TSubclassOf<class ATinoCharacter> OverlapFilter;
	UPROPERTY(VisibleAnywhere, Category = "Stat")
		TSet<AActor*> OverlappingActors;
};
