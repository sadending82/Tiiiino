// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "TinoController.generated.h"

/**
 * 
 */
enum class ETimerType : uint8;

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
	void ChangeMenuWidget(UUserWidget* NewWidgetClass);
	
	void CreateDummy();

	void SetInputUIMode();
	void SetInputGameMode();
	
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void OpenInGameUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void RemoveDialogUI();

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void UIAlertMessage(EDialogUICheck check);

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	UUserWidget* GetCurrentWidget() { return CurrentWidget; }

	bool bIsLobbyConnected = false;
public:
	// Network Disconnect
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void DisconnectNetwork();

public:
	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void ShowGameResult(int level, int rank, double grade, int point);

	UFUNCTION(Blueprintcallable, Category = "UMG_Game")
	void InitializeUIInstance();

	//UI 관련 함수
	void TimerStart(ETimerType type);
	void TimerRun();
protected:
	virtual void BeginPlay() override;

public:
	//UI 관련 변수

	// LoginUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class ULoginUIWidget> LoginWidgetClass;
	UPROPERTY()
	class ULoginUIWidget* LoginUIInstance = nullptr;

	// LobbyUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class ULobbyUIWidget> LobbyWidgetClass;
	UPROPERTY()
	class ULobbyUIWidget* LobbyUIInstance = nullptr;

	// CreateAccountsUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UCreateAccountsWidget> CreateAccountsWidgetClass;
	UPROPERTY()
	class UCreateAccountsWidget* CreateAccountsUIInstance = nullptr;

	// DialogUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UDialogUIWidget> DialogWidgetClass;
	UPROPERTY()
	class UDialogUIWidget* DialogUIInstance = nullptr;

	// InGameTimerUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UInGameTimerWidget> InGameUITimerWidgetClass;
	UPROPERTY()
	class UInGameTimerWidget* InGameUITimerInstance = nullptr;

	// InGameUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UInGameUIWidget> InGameWidgetClass;
	UPROPERTY()
	class UInGameUIWidget* InGameUIInstance = nullptr;

	// FinishGameUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UFinishGameUIWidget> FinishGameUIWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UFinishGameUIWidget* FinishGameUIInstance = nullptr;
	
	// StoreUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UStoreUIWidget> StoreUIWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UStoreUIWidget* StoreUIInstance = nullptr;

	// InventoryUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UInventoryUIWidget> InventoryUIWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UInventoryUIWidget* InventoryUIInstance = nullptr;

	// NoticeUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UNoticeUIWidget> NoticeUIWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UNoticeUIWidget* NoticeUIInstance = nullptr;


	// CouponUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UCouponUIWidget> CouponUIWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UCouponUIWidget* CouponUIInstance = nullptr;

	// CouponDialogUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
	TSubclassOf<class UCouponDialogWidget> CouponDialogUIWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UMG_Game")
	class UCouponDialogWidget* CouponDialogUIInstance = nullptr;

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
		
	FTimerHandle UITimerHandle;

	UPROPERTY()
	ETimerType Type;

};
