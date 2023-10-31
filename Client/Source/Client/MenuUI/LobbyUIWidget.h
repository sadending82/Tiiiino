// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUIWidget.generated.h"

/**
 * 
 */



UCLASS()
class CLIENT_API ULobbyUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
public:
	// 게임 준비
	UPROPERTY(meta = (BindWidget))
	class UButton* GameStartButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryGameStart();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryGameReady();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryGameReadyCancel();

	

public:
	// 로그인 화면으로 돌아가기
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryBack();
	
	// 상점으로 이동
	UPROPERTY(meta = (BindWidget))
	class UButton* StoreButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryEnterStore();

	// 인벤토리로 이동
	UPROPERTY(meta = (BindWidget))
	class UButton* InventoryButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryEnterInventory();
	
	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
		float Grade;
	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
		int32 Point;
public:
	// 랭크 시스템

	// 서버에서 보내주는 랭크데이터 받음
	void GetRankData(char DepartmentName[], char DepartmentPoint[]);

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	TArray<int> DepartmentNameArray = { 0,1,2,3,4,5,6,7,8,9 };

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
	TArray<int> DepartmentPointArray = { 0,1,2,3,4,5,6,7,8,9 };
};
