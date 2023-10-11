// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UCreditUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* EscapeButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void OpenCreditUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CloseCreditUI();

};
