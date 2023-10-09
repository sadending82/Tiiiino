// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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


	UFUNCTION()
	void TryNewAccounts();

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginIDTextBox;

	UFUNCTION()
	void InputLoginID(const FText& Text);

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginPasswordTextBox;

	UFUNCTION()
	void InputLoginPassword(const FText& Text);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<UUserWidget> NextWidgetClass;

	UPROPERTY()
	UUserWidget* CurrentWidget;
};
