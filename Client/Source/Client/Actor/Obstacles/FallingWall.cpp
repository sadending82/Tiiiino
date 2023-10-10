// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/Obstacles/FallingWall.h"
#include "Global.h"

#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"

#include "Actor/Character/BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Network/Network.h"


AFallingWall::AFallingWall()
	:RadialFalloffMagnitude(1000000.f),
	RadialVectorMagnitude(15000000.f)
{
	UHelpers::CreateComponent<USceneComponent>(this, &SceneComponent, "SceneComponent");
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &LeftMesh, "LeftMesh", SceneComponent);
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &RightMesh, "RightMesh", SceneComponent);
	UHelpers::CreateComponent<UGeometryCollectionComponent>(this, &LeftBreakableMesh, "LeftBreakableMesh", SceneComponent);
	UHelpers::CreateComponent<UGeometryCollectionComponent>(this, &RightBreakableMesh, "RightBreakableMesh", SceneComponent);

	UHelpers::CreateComponent<UFieldSystemComponent>(this, &FieldSystemComponent, "FieldSystemComponent", SceneComponent);

	RadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("RadialFalloff"));
	RadialVector = CreateDefaultSubobject<URadialVector>(TEXT("RadialVector"));
	MetaData = CreateDefaultSubobject<UFieldSystemMetaDataFilter>(TEXT("MetaData"));

	FieldLocation = GetActorLocation();
}

void AFallingWall::BeginPlay()
{
	Super::BeginPlay();

	//파괴되는 버전
	if (bIsBreakable)
	{
		//일반 메쉬 가림
		LeftMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LeftMesh->SetVisibility(false);
		RightMesh->SetVisibility(false);

		//파괴 메시에 히트이벤트 추가
		LeftBreakableMesh->OnComponentHit.AddDynamic(this, &AFallingWall::MeshComponentHit);
		RightBreakableMesh->OnComponentHit.AddDynamic(this, &AFallingWall::MeshComponentHit);

	}
	else
	{
		//파괴용 메시 가림
		LeftBreakableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightBreakableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		LeftBreakableMesh->bHiddenInGame = true;
		RightBreakableMesh->bHiddenInGame = true;
	}
}

void AFallingWall::MeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto OverlapCharacter = Cast<ABaseCharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{

		if (OverlapCharacter->bIsControlledPlayer)
		{
			//send hit packet
			send_game_breakdoor_packet(Network::GetNetwork()->s_socket, mObjectID);
			FieldLocation = OverlapCharacter->GetActorLocation();
			ActionObject();
		}
		else
		{

		}
	}
}

void AFallingWall::ActionObject()
{
	MetaData->ObjectType = EFieldObjectType::Field_Object_Destruction;
	RadialFalloff->SetRadialFalloff(RadialFalloffMagnitude, 0.8f, 1.0f, 0.f, 200.f, FieldLocation, Field_Falloff_Linear);
	RadialVector->SetRadialVector(RadialVectorMagnitude, FieldLocation);

	FieldSystemComponent->ApplyPhysicsField(true, EFieldPhysicsType::Field_ExternalClusterStrain, nullptr, RadialFalloff);
	FieldSystemComponent->ApplyPhysicsField(true, EFieldPhysicsType::Field_LinearForce, MetaData, RadialVector);
}
