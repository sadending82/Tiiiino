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
		//�޽ô� ���� ĳ�����̵� ������ �����ϴ� ĳ�����̵� ĳ���Ϳ� ���� �и� ����
		//Ŭ�󿡼� �˾Ƽ� ��Ʈ �̺�Ʈ �߻�

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
			//��Ʈ �߻��� �Ѿ����� �߰���
			//���� �޽� �����ϰ� ������Ʈ���ݷ������� ��ü��ų��
			//Ÿ�̸� ����ϱ�

			if (Controller->IsPlayerController())
			{
				

			}
		}
		else
			CLog::Log("Controller is nullptr");
	}
}
