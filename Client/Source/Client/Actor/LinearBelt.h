// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LinearBelt.generated.h"

UCLASS()
class CLIENT_API ALinearBelt : public AActor
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

	UFUNCTION()
		void BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void BoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
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

	UPROPERTY(VisibleAnywhere, Category = "Stat")
		TSet<ACharacter*> OverlappingActors;
};
