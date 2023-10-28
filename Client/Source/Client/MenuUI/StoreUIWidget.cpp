// Fill out your copyright notice in the Description page of Project Settings.



#include "MenuUI/StoreUIWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Network/Network.h"
#include "Global.h"


void UStoreUIWidget::NativePreConstruct()
{
}

void UStoreUIWidget::NativeDestruct()
{
}

void UStoreUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	TinoController->ChangeMenuWidget(TinoController->LobbyUIInstance);
}

void UStoreUIWidget::PurchaseItem()
{

}

void UStoreUIWidget::MoveLeft()
{
}

void UStoreUIWidget::MoveRight()
{
}
