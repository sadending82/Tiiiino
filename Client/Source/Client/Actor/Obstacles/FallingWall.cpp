// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/Obstacles/FallingWall.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

AFallingWall::AFallingWall()
{
	UHelpers::CreateComponent<USceneComponent>(this, &SceneComponent, "SceneComponent");
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &LeftMesh, "LeftMesh", SceneComponent);
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &RightMesh, "RightMesh", SceneComponent);
	UHelpers::CreateComponent<UGeometryCollectionComponent>(this, &LeftBreakableMesh, "LeftBreakableMesh", SceneComponent);
	UHelpers::CreateComponent<UGeometryCollectionComponent>(this, &RightBreakableMesh, "RightBreakableMesh", SceneComponent);

	LeftBreakableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightBreakableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftBreakableMesh->bHiddenInGame = true;
	RightBreakableMesh->bHiddenInGame = true;
}

void AFallingWall::BeginPlay()
{
	Super::BeginPlay();

	if (bIsFallable)
	{
		//메시는 조종 캐릭터이든 서버가 통제하는 캐릭터이든 캐릭터에 의해 밀릴 것임
		//클라에서 알아서 히트 이벤트 발생

		LeftMesh->SetSimulatePhysics(true);
		RightMesh->SetSimulatePhysics(true);
		LeftMesh->SetMobility(EComponentMobility::Movable);
		RightMesh->SetMobility(EComponentMobility::Movable);

		LeftMesh->OnComponentHit.AddDynamic(this, &AFallingWall::MeshComponentHit);
		RightMesh->OnComponentHit.AddDynamic(this, &AFallingWall::MeshComponentHit);
	}
}

void AFallingWall::MeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto OverlapCharacter = Cast<ACharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
		auto Controller = OverlapCharacter->GetController();
		if (!!Controller)
		{
			//히트 발생시 넘어지는 중간에
			//기존 메시 삭제하고 지오메트리콜렉션으로 교체시킬것
			//타이머 사용하기

			if (Controller->IsPlayerController())
			{
				

			}
		}
		else
			CLog::Log("Controller is nullptr");
	}
}
