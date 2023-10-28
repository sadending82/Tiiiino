// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InventoryUIWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Network/Network.h"
#include "Global.h"

void UInventoryUIWidget::NativePreConstruct()
{
}

void UInventoryUIWidget::NativeDestruct()
{
}

void UInventoryUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	TinoController->ChangeMenuWidget(TinoController->LobbyUIInstance);
}
