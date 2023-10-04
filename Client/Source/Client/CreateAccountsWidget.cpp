// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateAccountsWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Components/EditableTextBox.h"
#include "Network/Network.h"
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
	const char* id = TCHAR_TO_ANSI(*CreateIDTextBox->GetText().ToString());
	const char* pass = TCHAR_TO_ANSI(*CreatePasswordTextBox->GetText().ToString());
	Network::GetNetwork()->MyCharacterName = id;
	Network::GetNetwork()->MyCharacterPassWord = pass;
	send_newaccount_packet(Network::GetNetwork()->l_socket, id, pass);
}

void UCreateAccountsWidget::InputCreateID(const FText& Text)
{
}

void UCreateAccountsWidget::InputCreatePassword(const FText& Text)
{
}
