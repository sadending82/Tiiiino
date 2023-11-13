#include "Component/InventoryComponent.h"
#include "Actor/Accessory/AccessoryItem.h"
#include "Network/Network.h"

#include "Global.h"

UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	EquipInfo.Add({ EEquipType::EEquipType_Back,nullptr });
	EquipInfo.Add({ EEquipType::EEquipType_Face,nullptr });
	EquipInfo.Add({ EEquipType::EEquipType_Hand,nullptr });
	EquipInfo.Add({ EEquipType::EEquipType_Head,nullptr });
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
	ClearEquippedInfo();
	InventoryContents.Empty();
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::ClearEquippedInfo()
{
	for (auto& [code, Item] : InventoryContents)
	{
		Item.bEquipped = false;
		if (Item.ItemInstance)
		{
			Item.ItemInstance->UnEquip();
			Item.ItemInstance = nullptr;
			EquipInfo[Item.ItemInfo.EquipType] = nullptr;
		}
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

	if (EquipInfo.IsEmpty()) return;
	//장착 부위별로 장착된 아이템을 확인
	auto& EquipSlot = EquipInfo[InventoryContents[ItemCode].ItemInfo.EquipType];
	//다른 부위가 이미장착중이면 탈착후에 재장착
	if (EquipSlot != nullptr)
	{
		EquipSlot->UnEquip();
		send_unequip_packet(Network::GetNetwork()->l_socket, StaticCast<int>(ItemCode));
	}
	EquipSlot = InventoryContents[ItemCode].ItemInstance;

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
