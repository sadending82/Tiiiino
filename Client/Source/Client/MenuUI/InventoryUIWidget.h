// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UInventoryUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;


	// 로비로 돌아가기
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryBack();

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	float Grade;
	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	float Point;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void UpdatePointAndPoint();

public:
	// 아이템 아이콘 클릭
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ClickItemIcon(const int itemcode);
	
	// 클릭한 아이템 장작
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void EquipClickedItem();

	// 아이템 장착여부 확인
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	bool CheckItemEquiped(const int64 itemcode);
};
