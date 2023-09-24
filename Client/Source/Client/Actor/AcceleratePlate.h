// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Global.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AcceleratePlate.generated.h"



UCLASS()
class CLIENT_API AAcceleratePlate : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAcceleratePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* MainMesh;
	UPROPERTY(EditAnywhere) UBoxComponent* OverlapMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
