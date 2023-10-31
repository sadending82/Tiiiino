// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/CreateAccountsWidget.h"
#include "MenuUI/LoginUIWidget.h"
#include "Actor/Controller/TinoController.h"
#include "Components/EditableTextBox.h"
#include "Network/Network.h"
#include "Global.h"

void UCreateAccountsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UCreateAccountsWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UCreateAccountsWidget::TryBack()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->ChangeMenuWidget(TinoController->LoginUIInstance);
}

void UCreateAccountsWidget::TryCreateAccounts()
{
	const char* id = TCHAR_TO_ANSI(*CreateIDTextBox->GetText().ToString());
	const char* pass = TCHAR_TO_ANSI(*CreatePasswordTextBox->GetText().ToString());
	Network::GetNetwork()->MyCharacterName = id;
	Network::GetNetwork()->MyCharacterPassWord = pass;
	send_newaccount_packet(Network::GetNetwork()->l_socket, id, pass, DepartmentNum + 1);
}

void UCreateAccountsWidget::InputCreateID(const FText& Text)
{
}

void UCreateAccountsWidget::InputCreatePassword(const FText& Text)
{
}

void UCreateAccountsWidget::CheckCreateAccount(bool check)
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		if (check == true)
			TinoController->UIAlertMessage(EDialogUICheck::EDC_CASuccess);

		if (check == false)
			TinoController->UIAlertMessage(EDialogUICheck::EDC_CAFailed);
	}
}

void UCreateAccountsWidget::CheckIDTextLength()
{
	// ID 글자수 제한
	FText cur_text = CreateIDTextBox->GetText();
	FString OriginalText = cur_text.ToString();
	FString ClippedText = OriginalText.Left(MaxLength);
	//LimitedText = FText::FromString(ClippedText);
	cur_text = FText::FromString(ClippedText);
	CreateIDTextBox->SetText(cur_text);
}

void UCreateAccountsWidget::CheckPasswordTextLength()
{
	FText cur_text = CreatePasswordTextBox->GetText();
	FString OriginalText = cur_text.ToString();
	FString ClippedText = OriginalText.Left(MaxLength);
	//LimitedText = FText::FromString(ClippedText);
	cur_text = FText::FromString(ClippedText);
	CreatePasswordTextBox->SetText(cur_text);
}

