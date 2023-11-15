// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUI/CreditUIWidget.h"

#include "LoginUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API ULoginUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* NewAccountsButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryLogin();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void UIAlertMessage();


	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryNewAccounts();

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginIDTextBox;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void InputLoginID(const FText& Text);

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginPasswordTextBox;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void InputLoginPassword(const FText& Text);

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CheckIDTextLength();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CheckPasswordTextLength();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CheckIDTextBox();
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CheckIDTextBoxLastword();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CheckPasswordTextBox();
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CheckPasswordTextBoxLastword();

public:
	// CreditUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	class TSubclassOf<UCreditUIWidget> CreditUIClass;

	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UCreditUIWidget* CreditUI;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreditButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ShowCreditUI();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<UUserWidget> NextWidgetClass;

	UPROPERTY()
	UUserWidget* CurrentWidget;

	UPROPERTY()
	int32 MaxLength = 10;
};
