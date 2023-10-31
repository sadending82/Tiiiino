// Fill out your copyright notice in the Description page of Project Settings.



#include "MenuUI/StoreUIWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "Components/TextBlock.h"
#include "Data/ItemData.h"
#include "Network/Network.h"
#include "Global.h"


void UStoreUIWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	//auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
	//Grade = TinoCharacter->GetGrade();
	//Point = TinoCharacter->GetPoint();


}

void UStoreUIWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UStoreUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	TinoController->ChangeMenuWidget(TinoController->LobbyUIInstance);
}

void UStoreUIWidget::PurchaseItem()
{
	// Buy버튼 클릭
	send_buyitem_packet(Network::GetNetwork()->l_socket, ClickItemCode);
	// 포인트 변화 적용

}

bool UStoreUIWidget::QualifyingPurchase(int64 itemcode)
{
	// 게임데이터매니저에서 아이템데이터 불러오기
	auto itemdata = Network::GetNetwork()->mGameDataManager->GetShopProductsList();
	int price = 0;
	int grade = 0;
	for (auto& data : itemdata)
	{
		if (itemcode == data.second.itemCode)
		{
			price = data.second.price;
			grade = data.second.cutline;
			break;
		}
	}
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
		if (!!TinoCharacter)
		{
			if (TinoCharacter->GetPoint() < price)
			{
				return false;
			}
			if (TinoCharacter->GetGrade() < grade)
			{
				return false;
			}
		}
	}

	return true;
}

void UStoreUIWidget::AlreadyPurchase()
{
	ShowPurchaseWarning(true);
}

void UStoreUIWidget::LimitGrade()
{
	ShowPurchaseWarning(false);
}

void UStoreUIWidget::ShowPurchaseWarning(int64 itemcode)
{
	auto itemdata = Network::GetNetwork()->mGameDataManager->GetShopProductsList();
	int price = 0;
	int grade = 0;
	for (auto& data : itemdata)
	{
		if (itemcode == data.second.itemCode)
		{
			price = data.second.price;
			grade = data.second.cutline;
			break;
		}
	}
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
		if (!!TinoCharacter)
		{
			if (TinoCharacter->GetPoint() < price)
			{
				warning = EPurchaseState::EPS_AlreadyPurchase;
				return;
			}
			if (TinoCharacter->GetGrade() < grade)
			{
				warning = EPurchaseState::EPS_LimitGrade;
				return;
			}
		}
	}
	warning = EPurchaseState::EPS_Purchase;
}

void UStoreUIWidget::MoveLeft()
{
	// 좌측 이동버튼 클릭
	
	
}

void UStoreUIWidget::MoveRight()
{
	// 우측 이동버튼 클릭
}
