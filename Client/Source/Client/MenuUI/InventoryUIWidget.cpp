// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InventoryUIWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "Actor/Accessory/AccessoryItem.h"
#include "Data/ItemData.h"

#include "Network/Network.h"
#include "Global.h"

void UInventoryUIWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	//TinoCharacter에서 GetInventoryContents함수로
	//FAssetData.Icon을 버튼 이미지로 설정
	//블루프린트,C++ 둘다 가능
	//인벤토리 UI에 띄워줄 아이템 목록 인덱스 시작, 마지막값을 저장
	//0~8 표시 (1페이지), 9 ~ 17 (2페이지)float
	//표시할 페이지 만큼 인벤토리가 안차있으면 기본 버튼 이미지를 보여주자
}

void UInventoryUIWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UInventoryUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	TinoController->ChangeMenuWidget(TinoController->LobbyUIInstance);
	//버튼에 연결된 인덱스로 인벤토리에서 아이템 코드 얻기 -> 여기까지는 클라에서해야함
	//아이템 코드 패킷 전송(아직 해당 함수는 없는듯?)
}

void UInventoryUIWidget::UpdatePointAndPoint()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
		if (!!TinoCharacter)
		{
			Point = TinoCharacter->GetPoint();
			Grade = TinoCharacter->GetGrade();
		}
	}
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

	for (auto& p : Network::GetNetwork()->mMyCharacter->AccessoryInventory)
	{
		
	}
	return false;
}
