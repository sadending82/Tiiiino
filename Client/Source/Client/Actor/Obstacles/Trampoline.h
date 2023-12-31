// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "Trampoline.generated.h"

UCLASS()
class CLIENT_API ATrampoline : public ABaseObstacle
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrampoline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* JumpingMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMeshComponent* CollisionMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation") UAnimationAsset* HitAnimation;

private:

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float ElasticForce = 3000.0f;

};
