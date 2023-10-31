// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreUIWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EPurchaseState : uint8
{
	EPS_Purchase UMETA(DisplayName = "Purchase"),
	EPS_AlreadyPurchase UMETA(DisplayName = "Already"),
	EPS_LimitGrade UMETA(DisplayName = "LimitGrade"),
	
};

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

	// 상점 Buy버튼 입력시 StoreDialog 창 띄움
	UFUNCTION(BluePrintImplementableEvent, Category = "UMG_Game")
	void ShowStoreDialog();

	// 상점 아이템 구매 가능 확인
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	bool QualifyingPurchase(int64 itemcode);
	
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void AlreadyPurchase();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LimitGrade();

	// 아이템 구매 불가 사유 Dialog
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ShowPurchaseWarning(int64 itemcode);

	UFUNCTION(BluePrintImplementableEvent, Category = "UMG_Game")
	void ChangePoint();

	// StoreDialog
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UStoreUIWidget> StoreDialogWidgetClass;
	UPROPERTY()
	class UStoreUIWidget* StoreDialogInstance = nullptr;

	// StoreBuyResult
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UStoreUIWidget> StoreBuyResultWidgetClass;
	UPROPERTY()
	class UStoreUIWidget* StoreBuyResultInstance = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	TArray<int> ShopItemCodeList;

public:
	// 좌/우 아이템 칸 이동
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void MoveLeft();
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void MoveRight();

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	float Grade;
	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	float Point;

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	int64 ClickItemCode;

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	EPurchaseState warning;

};
