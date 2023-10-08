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

public:
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
	UPROPERTY(meta = (BindWidget))
	class UButton* InGameMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InGameMenuButtonText;

	UPROPERTY(meta = (BindWidget))
	class UImage* InGameMenuImage;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void OpenInGameMenuUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CloseInGameMenuUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void DisconnectGame();

};
