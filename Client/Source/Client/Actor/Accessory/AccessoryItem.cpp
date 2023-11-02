// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Accessory/AccessoryItem.h"
#include "Actor/Character/TinoCharacter.h"
#include "Global.h"

#include "Network/Network.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAccessoryItem::AAccessoryItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	UHelpers::CreateComponent(this, &SceneComponent, "SceneComponent");
	UHelpers::CreateComponent(this, &MeshComponent, "Mesh", SceneComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AAccessoryItem::BeginPlay()
{
	Super::BeginPlay();
}

void AAccessoryItem::SetSocketNameWithItemCode(const int itemCode)
{
	if (itemCode < 0) return;
	else if (itemCode >= STARTCODE_BACKEQUIP && itemCode < STARTCODE_HANDEQUIP) SocketName = "BackAccessory";
	else if (itemCode >= STARTCODE_HANDEQUIP && itemCode < STARTCODE_FACEEQUIP) SocketName = "HandAccessory";
	else if (itemCode >= STARTCODE_FACEEQUIP && itemCode < STARTCODE_HEADEQUIP) SocketName = "EyeAccessory";
	else if (itemCode >= STARTCODE_HEADEQUIP) SocketName = "HeadAccessory";
	else return;

}
