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
	// IDText의 크기가 0일때 아이디 생성 방지
	FText cur_text = CreateIDTextBox->GetText();
	FString OriginalText = cur_text.ToString();
	if (OriginalText.Len() == 0)
	{
		CheckCreateAccount(false);
		return;
	}
	// PasswordText의 크기가 0일때 아이디 생성 방지
	cur_text = CreatePasswordTextBox->GetText();
	OriginalText = cur_text.ToString();
	if (OriginalText.Len() == 0)
	{
		CheckCreateAccount(false);
		return;
	}

	const char* id = TCHAR_TO_ANSI(*CreateIDTextBox->GetText().ToString());
	const char* pass = TCHAR_TO_ANSI(*CreatePasswordTextBox->GetText().ToString());
	Network::GetNetwork()->MyCharacterName = id;
	Network::GetNetwork()->MyCharacterPassWord = pass;
	send_newaccount_packet(Network::GetNetwork()->l_socket, id, pass, DepartmentNum + 1);
}

void UCreateAccountsWidget::InputCreateID()
{
	// 이름은 Input이지만 실제 기능은 스페이스바, 한글체크
	FText cur_text = CreateIDTextBox->GetText();
	FString OriginalText = cur_text.ToString();
	int32 length = OriginalText.Len() - 1;
	if (length >= 0)
	{
		if (OriginalText[length] >= 'a' && OriginalText[length] <= 'z')
		{
			return;
		}
		else if (OriginalText[length] >= 'A' && OriginalText[length] <= 'Z')
		{
			return;
		}
		else if (OriginalText[length] >= '0' && OriginalText[length] <= '9')
		{
			return;
		}
		else
		{
			cur_text = FText::FromString(OriginalText.Left(length));
			
			CreateIDTextBox->SetText(cur_text);
		}
	}
}

void UCreateAccountsWidget::InputCreatePassword()
{
	// 이름은 Input이지만 실제 기능은 스페이스바, 한글체크
	FText cur_text = CreatePasswordTextBox->GetText();
	FString OriginalText = cur_text.ToString();
	int32 length = OriginalText.Len() - 1;
	if (length >= 0)
	{
		if (OriginalText[length] >= 'a' && OriginalText[length] <= 'z')
		{
			return;
		}
		else if (OriginalText[length] >= 'A' && OriginalText[length] <= 'Z')
		{
			return;
		}
		else if (OriginalText[length] >= '0' && OriginalText[length] <= '9')
		{
			return;
		}
		else
		{
			cur_text = FText::FromString(OriginalText.Left(length));
			CreatePasswordTextBox->SetText(cur_text);
		}
	}
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

