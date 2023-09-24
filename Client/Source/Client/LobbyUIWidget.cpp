// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUIWidget.h"
#include "Actor/Controller/TinoController.h"

#include "Global.h"

void ULobbyUIWidget::NativePreConstruct()
{
}

void ULobbyUIWidget::NativeDestruct()
{
}

void ULobbyUIWidget::TryGameStart()
{
	// GameStart Button Onclicked


}

void ULobbyUIWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->ChangeMenuWidget(TinoController->GetLoginWidgetClass());
}
