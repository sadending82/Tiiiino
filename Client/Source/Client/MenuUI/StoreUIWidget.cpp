// Fill out your copyright notice in the Description page of Project Settings.



#include "MenuUI/StoreUIWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "Components/TextBlock.h"
#include "Data/ItemData.h"
#include "Network/Network.h"
#include "Component/InventoryComponent.h"
#include "ClientGameMode.h"
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

	TinoController->LobbyUIInstance->UpdatePointAndGrade();

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
	// 게임데이터매니저에서 아이템데이터 불러오기, 게임모드 초기화시 게임데이터매니저 데이터로 초기화되어있음
	if (GameModeInstance == nullptr)
	{
		CLog::Log("GameMode Instance is Nullptr");
		InitInstance();
	}

	TArray<int> ShopProductTable;
	GameModeInstance->GetShopProductTable(ShopProductTable);
	int price = 0;
	float grade = 0;

	for (auto& code : ShopProductTable)
	{
		if (itemcode == code)
		{
			auto data = GameModeInstance->GetShopProductData(code);
			price = data->SellValue;
			grade = data->NumericData.PurchaseCondition;
			break;
		}
	}

	if (!!PlayerInstance)
	{
		if (PlayerInstance->GetPoint() < price)
		{
			return false;
		}
		if (PlayerInstance->GetGrade() < grade)
		{
			return false;
		}
		for (auto& p : PlayerInstance->GetInventoryContents())
		{
			if (p.ItemInfo.ItemCode == itemcode)
				return false;
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
	if (GameModeInstance == nullptr)
	{
		CLog::Log("GameMode Instance is Nullptr");
		InitInstance();
	}

	TArray<int> ShopProductTable;
	GameModeInstance->GetShopProductTable(ShopProductTable);

	int price = 0;
	float grade = 0;

	for (auto& code : ShopProductTable)
	{
		if (itemcode == code)
		{
			auto data = GameModeInstance->GetShopProductData(code);
			price = data->SellValue;
			grade = data->NumericData.PurchaseCondition;
			break;
		}
	}


	auto inventory = PlayerInstance->GetInventoryContents();
	if (!!PlayerInstance)
	{
		for (auto& p : inventory)
		{
			if (p.ItemInfo.ItemCode == itemcode)
			{
				warning = EPurchaseState::EPS_AlreadyPurchase;
				return;
			}
		}
		if (PlayerInstance->GetGrade() < grade)
		{
			warning = EPurchaseState::EPS_LimitGrade;
			return;
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

void UStoreUIWidget::UpdatePointAndGrade()
{
	if (PlayerInstance == nullptr)
		InitInstance();
	if (!!PlayerInstance)
	{
		Point = PlayerInstance->GetPoint();
		Grade = PlayerInstance->GetGrade();
	}
}

void UStoreUIWidget::InitInstance()
{
	PlayerInstance = Cast<ATinoCharacter>(GetOwningPlayerPawn());
	GameModeInstance = Cast<AClientGameMode>(UGameplayStatics::GetGameMode(PlayerInstance->GetWorld()));

}
