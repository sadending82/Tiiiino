// Fill out your copyright notice in the Description page of Project Settings.



#include "MenuUI/StoreUIWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "Components/TextBlock.h"
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
	int32 price = 100;
	Point -= price;
	ChangePoint();
	send_buyitem_packet(Network::GetNetwork()->l_socket, ClickItemCode);

	// Store BuyResult 창 띄움
	// StoreBuyResultInstance->AddToViewport();
}

bool UStoreUIWidget::QualifyingPurchase()
{
	// 학점 제한여부 확인
	if (true)
	{
		
		//ShowPurchaseWarning(false);
	}

	// 보유 여부 확인
	if (true)
	{

		//ShowPurchaseWarning(true);
	}
	// 임시 게임데이터매니저에서 아이템데이터 불러오기
	auto itemdata = Network::GetNetwork()->mGameDataManager->GetShopProductsList();
	for (auto &data : itemdata)
	{
		int price = data.second.price;
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

void UStoreUIWidget::MoveLeft()
{
	// 좌측 이동버튼 클릭
}

void UStoreUIWidget::MoveRight()
{
	// 우측 이동버튼 클릭
}
