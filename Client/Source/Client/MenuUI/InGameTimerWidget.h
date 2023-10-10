// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameTimerWidget.generated.h"

UENUM(BlueprintType)
enum class ETimerType : uint8
{
	ETT_LevelClear UMETA(DisplayName = "Clear"),
	ETT_LevelStart UMETA(DisplayName = "Start"),

};

/**
 * 
 */
UCLASS()
class CLIENT_API UInGameTimerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

public:

	// 타이머가 시작될시
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerStart(ETimerType Type);

	// 타이머 진행중일시
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerRun(ETimerType Type);

	// 타이머 종료시
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TimerEnd(ETimerType Type);

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	int GetRestGameTime() const { return RestGameTime; }

private:
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	int RestGameTime = 20;
	
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	class UInGameUIWidget* InGameUI;

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	class TSubclassOf<UInGameUIWidget> InGameUIClass;

};
