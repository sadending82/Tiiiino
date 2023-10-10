// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UDialogUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ResetWindow();
public:
	// Server Disconnect UI
	UPROPERTY(meta = (BindWidget))
	class UButton* DisconnectNetworkButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisconnectNetworkText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisconnectNetworkButtonText;

	UPROPERTY(meta = (BindWidget))
	class UImage* DisconnectNetworkImage;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void DisconnectNetwork();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void RenderDisconnectNetworkWindow();

public:
	// InGameMenu UI
	UPROPERTY(meta = (BindWidget))
	class UButton* InGameMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InGameMenuButtonText;

	UPROPERTY(meta = (BindWidget))
	class UImage* InGameMenuImage;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void RenderInGameMenuUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void DisconnectGame();

public:
	// Login Failed UI Window
	UPROPERTY(meta = (BindWidget))
	class UButton* FailedButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FailedText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FailedButtonText;

	UPROPERTY(meta = (BindWidget))
	class UImage* FailedImage;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void RenderUIAlertMessage();

	// Set Login Failed UI
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void SetLoginFailedUI();

	// Set CreateAccount Success UI
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void SetCreateAccountSuccessUI();

	// Set CreateAccount Failed UI
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void SetCreateAccountFailedUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void FixedAlertMessage();

};
