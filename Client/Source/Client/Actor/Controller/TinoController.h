// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "TinoController.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EDialogUICheck : uint8
{
	EDC_LoginFailed,
	EDC_CASuccess,
	EDC_CAFailed,
};

UCLASS()
class CLIENT_API ATinoController : public APlayerController
{
	GENERATED_BODY()

public:
	ATinoController();

public:
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);
	
	void CreateDummy();

	void SetInputUIMode();
	void SetInputGameMode();
	
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void OpenInGameUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void RemoveDialogUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void UIAlertMessage(EDialogUICheck check);

	//APlayerController* GetController();
	TSubclassOf<UUserWidget> GetLobbyWidgetClass() const { return LobbyWidgetClass; }
	TSubclassOf<UUserWidget> GetStartingWidgetClass() const { return StartingWidgetClass; }
	TSubclassOf<UUserWidget> GetCreateAccountsWidgetClass() const { return CreateAccountsWidgetClass; }

	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UDialogUIWidget* DialogUI;

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	UUserWidget* GetCurrentWidget() { return CurrentWidget; }

public:
	// Network Disconnect
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void DisconnectNetwork();

	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UDialogUIWidget* DialogUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	class TSubclassOf<UDialogUIWidget> DialogUIClass;

public:
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ShowGameResult(int rank, double grade, int point);

	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UFinishGameUIWidget* FinishGameUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	class TSubclassOf<UFinishGameUIWidget> FinishGameUIClass;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<UUserWidget> LobbyWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<UUserWidget> CreateAccountsWidgetClass;
	
	

	UPROPERTY()
	UUserWidget* CurrentWidget;


private:
	
	UPROPERTY(EditAnywhere, Category = "Test")
		int32 MaxDummyCount;

	UPROPERTY(EditAnywhere, Category = "Test")
		int32 DummyOffset;

	UPROPERTY(EditAnywhere, Category = "Test")
		TArray<class ATinoCharacter*> DummyArray;

	UPROPERTY(EditAnywhere, Category = "BPClass")
		TSubclassOf<class ATinoCharacter> TinoCharacterClass;
		
};
