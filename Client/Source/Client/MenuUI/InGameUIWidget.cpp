// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InGameUIWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"

#include "Global.h"

void UInGameUIWidget::NativePreConstruct()
{
	RestGameTime = 20;
}

void UInGameUIWidget::NativeDestruct()
{
}

void UInGameUIWidget::LevelSuccess()
{
	// 완주 성공
	

}

void UInGameUIWidget::LevelFailed()
{
	// 완주 실패

}

void UInGameUIWidget::LevelClearCheck()
{
	// 완주 성공 실패 체크
	// 성공시 bLevelClearCheck = true
	// 실패시 bLevelClearCheck = false
	
}

void UInGameUIWidget::TimerStart()
{

	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
	if (TinoCharacter)
	{
		// ATinoCharacter 인스턴스를 사용하여 TimerStart 함수를 호출
		TinoCharacter->TimerStart();
	}
}

void UInGameUIWidget::TimerEnd()
{
	// 게임오버 타이머 종료
	RestGameTime = 20;
	
}

void UInGameUIWidget::TimerRun()
{
	// 게임오버 타이머 진행

	FText RestGameTimeText = FText::AsNumber(RestGameTime);
	GameTimeText->SetText(RestGameTimeText);
	RestGameTime--;
	

}

void UInGameUIWidget::DisconnectNetwork()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TinoController->DisconnectNetwork();
}

