// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(meta = (BindWidget))
	class UButton* EnterLobbyButton;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void EnterLobby();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	bool CheckResult() { return bCheckResult; }

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void SetCheckResult(bool bcheckresult);

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void SuccessGame();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void FailedGame();

private:
	UPROPERTY(EditAnywhere, Category = "UMG_Game")
	bool bCheckResult;
};
