// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateAccountsWidget.h"
#include "Actor/Controller/TinoController.h"

#include "Global.h"

void UCreateAccountsWidget::NativePreConstruct()
{
}

void UCreateAccountsWidget::NativeDestruct()
{
}

void UCreateAccountsWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->ChangeMenuWidget(TinoController->GetLoginWidgetClass());
}

void UCreateAccountsWidget::TryCreateAccounts()
{
}

void UCreateAccountsWidget::InputCreateID(const FText& Text)
{
}

void UCreateAccountsWidget::InputCreatePassword(const FText& Text)
{
}
