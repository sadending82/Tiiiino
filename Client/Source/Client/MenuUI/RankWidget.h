// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Actor.h"
#include "Network/Network.h"
#include "../Actor/Character/TinoCharacter.h"
#include "../Actor/GoalArea.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include <map>
#include <vector>
#include <algorithm>
#include "Global.h"
#include "RankWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API URankWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct();
public:
	
	UFUNCTION(BlueprintCallable)
	int GetMyCurrentRank() { return sCurrentRank; }

	UFUNCTION(BlueprintCallable)
	TArray<float>& GetCharacterDistanceData();

	void CalculateNewRank();

	void GetDataOfCharacters();

	void ChangePositionByDistanceData();

	virtual void NativeTick(const FGeometry& Geometry, float InDeltaTime) override;

	std::map<ATinoCharacter*, float> PtrBasedRankData;
	std::vector<std::pair<float, ATinoCharacter*>> RankData;

	TArray<float> CharacterDistanceData;

	UPROPERTY(BlueprintReadOnly) int sCurrentRank = -1;

	UPROPERTY()
	TArray<float> OtherCharacterDistanceData; // UI Àü´Þ¿ë

	UPROPERTY(BlueprintReadOnly)
	float myCharcaterDistanceData = 0.f;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* MyCharacterImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OtherCharacterImage0;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OtherCharacterImage1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OtherCharacterImage2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OtherCharacterImage3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OtherCharacterImage4;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OtherCharacterImage5;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* OtherCharacterImage6;


private:

	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true)) FVector StartPosition = { 0, -100.f, 0 };
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true)) FVector EndPosition = { 0, 0, 0 };
	float fTrackDistance = 0.0f;
	bool bIsLevelLoaded = false;
};
