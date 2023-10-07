// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginUIWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Actor/Controller/TinoController.h"
#include "GameFramework/Pawn.h"
#include "Network/Network.h"

#include "Global.h"

void ULoginUIWidget::NativePreConstruct()
{
	LoginButton->OnClicked.AddDynamic(this, &ULoginUIWidget::TryLogin);
	NewAccountsButton->OnClicked.AddDynamic(this, &ULoginUIWidget::TryNewAccounts);

	LoginIDTextBox->OnTextChanged.AddDynamic(this, &ULoginUIWidget::InputLoginID);
	LoginPasswordTextBox->OnTextChanged.AddDynamic(this, &ULoginUIWidget::InputLoginPassword);
}

void ULoginUIWidget::NativeDestruct()
{
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

	//auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//TinoController->ChangeMenuWidget(TinoController->GetLobbyWidgetClass());

}

void ULoginUIWidget::TryNewAccounts()
{
	// NewAccounts Button Onclicked
	// UE_LOG(LogTemp, Error, TEXT("UnExpected Tree NewAccounts"));

	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->ChangeMenuWidget(TinoController->GetCreateAccountsWidgetClass());
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

void ULoginUIWidget::DisconnectNetwork()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->DisconnectNetwork();
}
