// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NoticeUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UNoticeUIWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct();
public:
	UPROPERTY(BlueprintReadWrite) FString Notice = "";

	UFUNCTION(BlueprintCallable)
	FString GetNotice();

	UFUNCTION(BluePrintImplementableEvent)
	void SetVisibleNotice();

	void SetNotice(FString notice);

};
