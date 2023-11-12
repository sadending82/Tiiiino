#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ItemData.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

struct FItemData;
class AAccessoryItem;

//플레이어가 보유한 아이템 정보외에
//장착여부와 같이 추가적인 정보가 필요하다면
//이 구조체에 추가바람
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

public:

	FInventoryItem() = default;
	FInventoryItem(const FItemData& ItemData, bool bEquipped) 
		: ItemInstance(nullptr),ItemInfo(ItemData), bEquipped(bEquipped) 
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryData")
		AAccessoryItem* ItemInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryData")
		FItemData ItemInfo; //아이템정보만 저장되있다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryData")
		bool bEquipped;  //장착여부

	bool operator==(const FInventoryItem& OtherData) const
	{
		return this->ItemInfo.ItemCode == OtherData.ItemInfo.ItemCode;
	}
};

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
	//용도는 아이템 업데이트시 UI가자동업데이트되기위함(UI호출하지않고 이벤트로 알아서처리)
	FOnInventoryUpdated OnInventoryUpdated;

	//인벤토리에 존재하는 아이템을 전부 돌려주는 함수
	TArray<FInventoryItem> GetInventoryContents() const;
	//
	FInventoryItem GetItemFromItemCode(const int64& ItemCode) const;

	void AddItem(const FItemData& ItemData, bool bEquipped = false);
	void ClearInventory();
	void ClearEquippedInfo();

	UFUNCTION(BlueprintCallable)
	void SetEquipped(const int64& ItemCode, bool bEquipped);
	UFUNCTION(BlueprintCallable)
	bool GetEquipped(const int64& ItemCode) const;
	UFUNCTION(BlueprintCallable)
	AAccessoryItem* GetInstnace(const int64& ItemCode) const;
	UFUNCTION(BlueprintCallable)
	void SetInstnace(const int64& ItemCode, AAccessoryItem* Item);

protected:
	// 플레이어 인벤토리
	UPROPERTY(VisibleAnywhere,Category="Inventory")
	TMap<int64,FInventoryItem> InventoryContents;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
		TMap<EEquipType, AAccessoryItem*> EquipInfo;

		
};
