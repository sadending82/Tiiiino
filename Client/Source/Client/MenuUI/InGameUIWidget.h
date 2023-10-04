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
	class UEditableText* GameTimeText;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void RankImageSwitcher();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void SetGameTime(int32 GameTime);

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void EndGameTime();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	int32 GetGameTime() { return RestGameTime; }

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	UImage* GetImage() { return MyImage; }

private:
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	int32 RestGameTime;
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	UImage* MyImage;
	
};
