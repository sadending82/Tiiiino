// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CouponDialogWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UCouponDialogWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BluePrintImplementableEvent, Category = "UMG_Game")
	void SuccessUsingCoupon();

	UFUNCTION(BluePrintImplementableEvent, Category = "UMG_Game")
	void FailedUsingCoupon();
};
