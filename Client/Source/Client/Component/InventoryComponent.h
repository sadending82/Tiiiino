#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class AAccessoryItem;
struct FItemData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLIENT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:	

	//인벤토리내 아이템 추가되거나 제거시 브로드캐스트할 것
	FOnInventoryUpdated OnInventoryUpdated;

	FORCEINLINE TArray<FItemData> GetInventoryContents() const { return InventoryContents; }

	void AddItem(const FItemData& ItemData);

protected:

	// 플레이어 인벤토리
	UPROPERTY(EditDefaultsOnly,Category="Inventory")
	TArray<FItemData> InventoryContents;
		
};
