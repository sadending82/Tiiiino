// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CouponUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UCouponUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// ÄíÆù »ç¿ë
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryUsingCoupon();

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* CouponTextbox;
};
