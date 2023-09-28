// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/LinearBelt.h"
#include "Global.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ALinearBelt::ALinearBelt()
	:MaterialSpeedY(0.1f), BeltSpeed(165.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UHelpers::CreateComponent<USceneComponent>(this, &SceneComponent, "SceneComponent");
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &Mesh, "Mesh", SceneComponent);
	UHelpers::CreateComponent<UBoxComponent>(this, &OverlapBox, "OverlapBox", SceneComponent);
	UHelpers::CreateComponent<UArrowComponent>(this, &DirectionArrow, "DirectionArrow", SceneComponent);
}

// Called when the game starts or when spawned
void ALinearBelt::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceDynamic* MIBelt = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(1), this);
	MIBelt->SetVectorParameterValue(FName(TEXT("MaterialSpeed")), FLinearColor(0.f, MaterialSpeedY, 0.f, 0.f));
	Mesh->SetMaterial(1, MIBelt);

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ALinearBelt::BoxBeginOverlap);
	OverlapBox->OnComponentEndOverlap.AddDynamic(this, &ALinearBelt::BoxEndOverlap);
}

// Called every frame
void ALinearBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OverlappingActors.Num() > 0)
	{
		for (auto actor : OverlappingActors)
		{
			actor->AddActorWorldOffset(DirectionArrow->GetForwardVector() * BeltSpeed * DeltaTime);
		}
	}
}

void ALinearBelt::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapCharacter = Cast<ACharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
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

void ALinearBelt::BoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto OverlapCharacter = Cast<ACharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
		if(OverlappingActors.Num() > 0)
			OverlappingActors.Remove(OverlapCharacter);
	}
}

