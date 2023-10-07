// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "TinoController.generated.h"

/**
 * 
 */
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
	

	//APlayerController* GetController();
	TSubclassOf<UUserWidget> GetLobbyWidgetClass() const { return LobbyWidgetClass; }
	TSubclassOf<UUserWidget> GetStartingWidgetClass() const { return StartingWidgetClass; }
	TSubclassOf<UUserWidget> GetCreateAccountsWidgetClass() const { return CreateAccountsWidgetClass; }

public:
	// Network ¿¬°á ²÷±è
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void DisconnectNetwork();

	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UDialogUIWidget* DialogUI;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	class TSubclassOf<UDialogUIWidget> DialogUIClass;

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
