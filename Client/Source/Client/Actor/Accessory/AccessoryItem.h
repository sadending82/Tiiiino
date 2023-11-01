// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AccessoryItem.generated.h"

UCLASS()
class CLIENT_API AAccessoryItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAccessoryItem();

public:
	template<typename T>
	static AAccessoryItem* Spawn(class UWorld* InWorld, TSubclassOf<T> BPClass,class ACharacter* InOwner)
	{
		FActorSpawnParameters params;
		params.Owner = InOwner;

		return InWorld->SpawnActor<T>(BPClass,params);
	}

	//static AAccessoryItem* SpawnWithBP(class UWorld* InWorld, UClass* BPClass, class ACharacter* InOwner)
	//{
	//	FActorSpawnParameters params;
	//	params.Owner = InOwner;
	//
	//	return InWorld->SpawnActor<AAccessoryItem>(BPClass, params);
	//}
public:

	virtual void Equip();
	virtual void UnEquip();

	int GetItemCode() const { return mItemCode; }
	void SetItemCode(const int itemcode) { mItemCode = itemcode; }

	void SetSocketNameWithItemCode(const int itemCode);
	void SetSocketName(const FName name) { SocketName = name; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class USceneComponent* SceneComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Component")
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Owner")
		class ACharacter* OwnerCharacter;
	UPROPERTY(EditDefaultsOnly, Category = "Socket")
		FName SocketName;

	UPROPERTY(VisibleAnywhere, Category = "Attachment")
		bool bEquipped;

	UPROPERTY(VisibleAnywhere, Category = "Attachment")
		int mItemCode;
};
