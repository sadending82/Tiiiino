// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "RotatingPole.generated.h"

UCLASS()
class CLIENT_API ARotatingPole : public ABaseObstacle
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatingPole();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
		FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh;

private:
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float RotateSpeed = 30.0f;
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float ElasticForce = 500.0f;
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true)) bool DoElasticCollision = true;
};
