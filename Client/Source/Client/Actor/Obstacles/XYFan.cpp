// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/Obstacles/XYFan.h"
#include "Actor/Character/TinoCharacter.h"
#include "Global.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

AXYFan::AXYFan()
	:FanPower(100.f),
	FanRotateSpeed(500.f)
{
	PrimaryActorTick.bCanEverTick = true;

	UHelpers::CreateComponent<USceneComponent>(this, &SceneComponent, "SceneComponent");
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &CoverMesh, "CoverMesh", SceneComponent);
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &FanMesh, "FanMesh", CoverMesh);
	UHelpers::CreateComponent<UBoxComponent>(this, &OverlapBox, "OverlapBox", SceneComponent);
	UHelpers::CreateComponent<UArrowComponent>(this, &DirectionArrow, "DirectionArrow", SceneComponent);

}

void AXYFan::BeginPlay()
{
	Super::BeginPlay();

	
}

void AXYFan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsPlayWindSFX == false)
	{
		bIsPlayWindSFX = true;
		ASoundManager::GetSoundManager()->PlaySFXAtLocation(this, ESFXType::ESFXType_WindBlow, GetActorLocation());
	}
	//비주얼용 회전
	FanMesh->AddLocalRotation(FRotator(0.0f, FanRotateSpeed * DeltaTime, 0.0f));

	OverlapBox->GetOverlappingActors(OverlappingActors, OverlapFilter);
	if (OverlappingActors.Num() > 0)
	{
		
		for (auto actor : OverlappingActors)
		{
			actor->AddActorWorldOffset(DirectionArrow->GetForwardVector() * FanPower * DeltaTime, true);
		}
	}
}
