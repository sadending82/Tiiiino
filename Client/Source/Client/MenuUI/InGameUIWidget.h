// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Engine/Texture2D.h>
#include <Components/Image.h>
#include "InGameUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UInGameUIWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GameTimeText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StartCountDownText;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelSuccess();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelFailed();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelClearCheck();

	UFUNCTION(BluePrintImplementableEvent, Category = "UMG_Game")
	void ShowResultUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerStart();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	bool GetbLevelClearCheck() { return bLevelClearCheck; }

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	bool bLevelClearCheck = false;

	// 인게임메뉴창 열기
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void OpenInGameUI();

	// 인게임메뉴창 닫기
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CloseInGameUI();

	// 나가기 버튼을 눌렀을 때 로비로 나가는 함수
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ChangeLobbyUI();

	// 게임시작시 카운트다운 3..2..1
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelStartCountdown();

	// 게임시작시 카운트다운 3..2..1
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelStart();

	// 게임종료시 카운트다운
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelClearCountdown();

	// 타이머 문구 체인지 20->19->18
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerTextChange(int RestGameTime, ETimerType Type);

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	class UInGameTimerWidget* InGameTimer;

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	class TSubclassOf<UInGameTimerWidget> InGameTimerClass;

private:

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	UImage* MyImage;

	
	
	
};
