// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Accessory/AccessoryItem.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAccessoryItem::AAccessoryItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	UHelpers::CreateComponent(this, &MeshComponent, "Mesh");
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AAccessoryItem::BeginPlay()
{
	Super::BeginPlay();
}

void AAccessoryItem::Equip()
{
	if (bEquipped == true) return;
	bEquipped = true;

	//SetInstigator(OwnerCharacter->GetController());
	AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), SocketName);

}


void AAccessoryItem::UnEquip()
{
	if (bEquipped == false) return;
	bEquipped = false;

}
