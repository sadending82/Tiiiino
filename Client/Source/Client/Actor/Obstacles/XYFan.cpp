// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/Obstacles/XYFan.h"
#include "Global.h"

#include "GameFramework/Character.h"
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

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AXYFan::BoxBeginOverlap);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &AXYFan::BoxEndOverlap);

}

void AXYFan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//비주얼용 회전
	FanMesh->AddLocalRotation(FRotator(0.0f, FanRotateSpeed * DeltaTime, 0.0f));

	if (OverlappingActors.Num() > 0)
	{
		for (auto actor : OverlappingActors)
		{
			actor->AddActorWorldOffset(DirectionArrow->GetForwardVector() * FanPower * DeltaTime);
		}
	}
}

void AXYFan::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapCharacter = Cast<ACharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
		ASoundManager::GetSoundManager()->PlaySFXAtLocation(ESFXType::ESFXType_WindBlow, GetActorLocation());

		auto Controller = OverlapCharacter->GetController();
		if (!!Controller)
		{
			if (Controller->IsPlayerController())
				OverlappingActors.Add(OverlapCharacter);
		}
		else
			CLog::Log("Controller is nullptr");
	}
}

void AXYFan::BoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto OverlapCharacter = Cast<ACharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
		if (OverlappingActors.Num() > 0)
			OverlappingActors.Remove(OverlapCharacter);
	}
}
