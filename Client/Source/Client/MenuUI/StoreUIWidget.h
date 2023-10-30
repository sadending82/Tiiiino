// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreUIWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EQualifyingPurchase : uint8
{
	EQP_AlreadyPurchase,	// 이미 구입함
	EDC_GradeLimit,			// 학점 제한
	EDC_Possible,			// 구매 가능
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
	bool QualifyingPurchase();
	
	// 아이템 구매 불가 사유 Dialog
	UFUNCTION(BluePrintImplementableEvent, Category = "UMG_Game")
	void ShowPurchaseWarning(bool check);

	// StoreDialog
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UStoreUIWidget> StoreDialogWidgetClass;
	UPROPERTY()
	class UStoreUIWidget* StoreDialogInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "UMG_Game")
	int ItemNum;

public:
	// 좌/우 아이템 칸 이동
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void MoveLeft();
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void MoveRight();

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	float Grade;
	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	int32 Point;

private:
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	EQualifyingPurchase QualifyCheck = EQualifyingPurchase::EDC_Possible;
};
