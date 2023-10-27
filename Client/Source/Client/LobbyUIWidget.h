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

	UPROPERTY(meta = (BindWidget))
	class UButton* GameStartButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryGameStart();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryGameReady();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryGameReadyCancel();

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void TryBack();

	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
		float Grade;
	UPROPERTY(BlueprintReadWrite, Category = "UMG_Game")
		int32 Point;
};
