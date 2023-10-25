// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <array>
#include "Blueprint/UserWidget.h"
#include "FinishGameUIWidget.generated.h"



/**
 * 
 */
UCLASS()
class CLIENT_API UFinishGameUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeConstruct() override;


	UPROPERTY(meta = (BindWidget))
	class UButton* EnterLobbyButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void EnterLobby();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ShowResult(int level, int rank, double grade, int point);


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StageNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RankNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GradeNameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PointNameText;

	std::array<FString, 7 + 1> MapNameArray;

private:
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	bool bLevelClearCheck;

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	FString CurrentStage;

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	int CurrentRank;

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	float GainGrade;

	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	int32 GainPoint;
};
