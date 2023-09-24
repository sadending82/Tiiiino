// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginUIWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Actor/Controller/TinoController.h"
#include "GameFramework/Pawn.h"

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
	
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->ChangeMenuWidget(TinoController->GetNextWidgetClass());
}

void ULoginUIWidget::TryNewAccounts()
{
	// NewAccounts Button Onclicked
	// UE_LOG(LogTemp, Error, TEXT("UnExpected Tree NewAccounts"));
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
