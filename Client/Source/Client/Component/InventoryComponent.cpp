#include "Component/InventoryComponent.h"
#include "Actor/Accessory/AccessoryItem.h"
#include "Data/ItemData.h"

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

void UInventoryComponent::AddItem(const FItemData& ItemData)
{
	/*if (InventoryContents.Contains(ItemData))
		CLog::Log("Already have Item!");*/
	InventoryContents.AddUnique(ItemData);

	OnInventoryUpdated.Broadcast();
}
