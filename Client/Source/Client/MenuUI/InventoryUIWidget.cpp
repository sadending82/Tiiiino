// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InventoryUIWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "Actor/Accessory/AccessoryItem.h"
#include "Data/ItemData.h"
#include "Component/InventoryComponent.h"

#include "Network/Network.h"
#include "Global.h"

void UInventoryUIWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UInventoryUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Network::GetNetwork()->mMyCharacter->SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
}

void UInventoryUIWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UInventoryUIWidget::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);
	auto Mycharacter = Network::GetNetwork()->mMyCharacter;
	if(Mycharacter)
		Mycharacter->AddActorLocalRotation(FRotator(0.0f, InDeltaTime * 20.0f, 0.0f));
}

void UInventoryUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	TinoController->ChangeMenuWidget(TinoController->LobbyUIInstance);

}

void UInventoryUIWidget::UpdateGradeAndPoint()
{
	if (PlayerInstance == nullptr)
		InitInstance();
	if (!!PlayerInstance)
	{
		Point = PlayerInstance->GetPoint();
		Grade = PlayerInstance->GetGrade();
	}
}

FString UInventoryUIWidget::GetAccessoryItemCode()
{
	if (PlayerInstance == nullptr)
		InitInstance();
	FString fstr;
	for (FInventoryItem& p : PlayerInstance->GetInventoryContents())
	{
		FString temp = FString::FromInt(p.ItemInfo.ItemCode);
		fstr += temp;
		fstr += ' ';
	}
	return fstr;
}

void UInventoryUIWidget::ClickItemIcon(const int itemcode)
{
	// 서버에 클릭한 아이템 코드 전달

}

void UInventoryUIWidget::EquipClickedItem(const int itemcode)
{
	// 아이템 장착
	send_equip_packet(Network::GetNetwork()->l_socket, itemcode);
}

void UInventoryUIWidget::UnEquipClickedItem(const int itemcode)
{
	// 아이템 해제
	send_unequip_packet(Network::GetNetwork()->l_socket, itemcode);
}

bool UInventoryUIWidget::CheckItemEquiped(const int64 itemcode)
{
	//인벤토리에서 바로 장착여부를 얻을 수 있게 되었습니다.
	//따라서 이 함수는 사용되지않습니다.
	if (PlayerInstance == nullptr)
		InitInstance();
	for (auto p : PlayerInstance->GetInventoryContents())
	{
		//int AccessoryItemCode = p->GetItemCode();
		//if (AccessoryItemCode <= 15)
		//{
		//	if (equiptype != EEquipType::EEquipType_Back)
		//	{
		//		return true;
		//	}
		//}
		//else if (AccessoryItemCode <= 31)
		//{
		//	if (equiptype == EEquipType::EEquipType_Hand)
		//	{
		//		return true;
		//	}
		//}
		//else if (AccessoryItemCode <= 47)
		//{
		//	if (equiptype == EEquipType::EEquipType_Face)
		//	{
		//		return true;
		//	}
		//}
		//else if (AccessoryItemCode <= 63)
		//{
		//	if (equiptype == EEquipType::EEquipType_Head)
		//	{
		//		return true;
		//	}
		//}
		//else
		//{
		//	// error
		//}
		//
		if (p.bEquipped)
			return true;
	}
	return false;
}

void UInventoryUIWidget::InitInstance()
{
	PlayerInstance = Cast<ATinoCharacter>(GetOwningPlayerPawn());
}
