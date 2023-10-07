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

	UPROPERTY(meta = (BindWidget))
	class UButton* DisconnectNetworkButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void DisconnectNetwork();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DialogText;
};
