// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreateAccountsWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UCreateAccountsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryBack();

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateAccountsButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryCreateAccounts();

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* CreateIDTextBox;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void InputCreateID(const FText& Text);

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* CreatePasswordTextBox;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void InputCreatePassword(const FText& Text);


};
