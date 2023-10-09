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

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelSuccess();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelFailed();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void LevelClearCheck();

	UFUNCTION(BluePrintImplementableEvent, Category = "UMG_Game")
	void ShowResultUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	bool GetbLevelClearCheck() { return bLevelClearCheck; }

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerStart();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerEnd();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerRun();


	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	int GetRestGameTime() const { return RestGameTime; }

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	bool bLevelClearCheck = false;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void OpenInGameUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void CloseInGameUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ChangeLobbyUI();

private:
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	int RestGameTime = 20;

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	UImage* MyImage;


	
	
};
