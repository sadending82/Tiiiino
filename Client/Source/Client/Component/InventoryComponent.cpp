#include "Component/InventoryComponent.h"
#include "Actor/Accessory/AccessoryItem.h"

#include "Global.h"

UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

TArray<FInventoryItem> UInventoryComponent::GetInventoryContents() const
{
	TArray<FInventoryItem> OutArray;
	InventoryContents.GenerateValueArray(OutArray);
	return OutArray;
}

FInventoryItem UInventoryComponent::GetItemFromItemCode(const int64& ItemCode) const
{
	if (InventoryContents.Contains(ItemCode) == false)
	{
		CLog::Log("Invalid ItemCode!");
		return FInventoryItem();
	}
	return InventoryContents[ItemCode];
}

void UInventoryComponent::SetInstnace(const int64& ItemCode, AAccessoryItem* Item)
{
	if (InventoryContents.Contains(ItemCode) == false)
	{
		CLog::Log("Don't Exist Item!");
		return;
	}
	InventoryContents[ItemCode].ItemInstance = Item;
}

void UInventoryComponent::AddItem(const FItemData& ItemData,bool bEquipped)
{
	if (InventoryContents.Contains(ItemData.ItemCode))
	{
		CLog::Log("Already have Item!");
		return;
	}
	InventoryContents.Add({ ItemData.ItemCode ,FInventoryItem(ItemData, bEquipped) });

	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::ClearInventory()
{
	InventoryContents.Empty();
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::ClearEquippedInfo()
{
	for (auto& [code, Item] : InventoryContents)
	{
		Item.bEquipped = false;
	}
}

void UInventoryComponent::SetEquipped(const int64& ItemCode, bool bEquipped)
{
	if (InventoryContents.Contains(ItemCode) == false)
	{
		CLog::Log("Invalid ItemCode!");
		return;
	}

	InventoryContents[ItemCode].bEquipped = bEquipped;
}

bool UInventoryComponent::GetEquipped(const int64& ItemCode) const
{
	if (InventoryContents.Contains(ItemCode) == false)
	{
		CLog::Log("Invalid ItemCode!");
		return false;
	}
	
	return InventoryContents[ItemCode].bEquipped;
}

AAccessoryItem* UInventoryComponent::GetInstnace(const int64& ItemCode) const
{
	if (InventoryContents.Contains(ItemCode) == false)
	{
		CLog::Log("Invalid ItemCode!");
		return nullptr;
	}

	return InventoryContents[ItemCode].ItemInstance;
}
