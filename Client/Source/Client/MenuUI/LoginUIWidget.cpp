// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/LoginUIWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "MenuUI/CreateAccountsWidget.h"
#include "Actor/Controller/TinoController.h"
#include "GameFramework/Pawn.h"
#include "Network/Network.h"

#include "Global.h"

void ULoginUIWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	LoginButton->OnClicked.AddDynamic(this, &ULoginUIWidget::TryLogin);
	NewAccountsButton->OnClicked.AddDynamic(this, &ULoginUIWidget::TryNewAccounts);

	LoginIDTextBox->OnTextChanged.AddDynamic(this, &ULoginUIWidget::InputLoginID);
	LoginPasswordTextBox->OnTextChanged.AddDynamic(this, &ULoginUIWidget::InputLoginPassword);
}

void ULoginUIWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULoginUIWidget::TryLogin()
{
	// Login Button Onclicked
	// UE_LOG(LogTemp, Error, TEXT("UnExpected Tree ID"));

	// Change Lobby Scene
	//ATinoController::ChangeMenuWidget(ATinoController::GetNextWidgetClass());
	const char* id = TCHAR_TO_ANSI(*LoginIDTextBox->GetText().ToString());
	const char* pass = TCHAR_TO_ANSI(*LoginPasswordTextBox->GetText().ToString());
	Network::GetNetwork()->MyCharacterName = id;
	Network::GetNetwork()->MyCharacterPassWord = pass;
	send_login_packet(Network::GetNetwork()->l_socket, id, pass);



}

void ULoginUIWidget::UIAlertMessage()
{
	// if login failed
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
		TinoController->UIAlertMessage(EDialogUICheck::EDC_LoginFailed);
}

void ULoginUIWidget::TryNewAccounts()
{
	// NewAccounts Button Onclicked
	// UE_LOG(LogTemp, Error, TEXT("UnExpected Tree NewAccounts"));

	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
		TinoController->ChangeMenuWidget(TinoController->CreateAccountsUIInstance);
}

void ULoginUIWidget::InputLoginID(const FText& Text)
{
	// Input ID Textbox
	// UE_LOG(LogTemp, Error, TEXT("UnExpected Tree IDInput"));
}

void ULoginUIWidget::InputLoginPassword(const FText& Text)
{
	// Input Password Textbox
	// UE_LOG(LogTemp, Error, TEXT("UnExpected Tree PasswordInput"));
}

void ULoginUIWidget::CheckIDTextLength()
{
	// ID 글자수 제한
	FText cur_text = LoginIDTextBox->GetText();
	FString OriginalText = cur_text.ToString();
	FString ClippedText = OriginalText.Left(MaxLength);
	//LimitedText = FText::FromString(ClippedText);
	cur_text = FText::FromString(ClippedText);
	LoginIDTextBox->SetText(cur_text);

	
}

void ULoginUIWidget::CheckPasswordTextLength()
{
	FText cur_text = LoginPasswordTextBox->GetText();
	FString OriginalText = cur_text.ToString();
	FString ClippedText = OriginalText.Left(MaxLength);
	//LimitedText = FText::FromString(ClippedText);
	cur_text = FText::FromString(ClippedText);
	LoginPasswordTextBox->SetText(cur_text);
}

void ULoginUIWidget::ShowCreditUI()
{
	CreditUI = Cast<UCreditUIWidget>(CreateWidget(GetWorld(), CreditUIClass));
	// Credit 버튼 클릭시 크레딧UI 창을 띄움
	if (!!CreditUI)
	{
		CreditUI->OpenCreditUI();
		CreditUI->AddToViewport();
	}

}

