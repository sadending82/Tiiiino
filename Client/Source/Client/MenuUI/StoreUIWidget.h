// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UStoreUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	// 로비로 돌아가기
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryBack();
public:
	// 상점 아이템 구매
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void PurchaseItem();

public:
	// 좌/우 아이템 칸 이동
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void MoveLeft();
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void MoveRight();

private:

};
