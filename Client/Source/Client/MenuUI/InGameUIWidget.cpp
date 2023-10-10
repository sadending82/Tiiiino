// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/InGameUIWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/TinoCharacter.h"
#include "MenuUI/InGameTimerWidget.h"
#include "Network/Network.h"

#include "Global.h"

void UInGameUIWidget::NativePreConstruct()
{

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

	// 레벨 종료시 마우스커서 띄우기
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		TinoController->SetShowMouseCursor(true);
	}
}

void UInGameUIWidget::TimerStart()
{
	InGameTimer = Cast<UInGameTimerWidget>(CreateWidget(GetWorld(), InGameTimerClass));
	StartCountDownText->SetRenderOpacity(1.0);
	if (!!InGameTimer)
	{
		InGameTimer->TimerStart(ETimerType::ETT_LevelClear);
	}
	
}
//
//void UInGameUIWidget::TimerEnd()
//{
//	// Timer end
//	// Reset RestGameTime
//	RestGameTime = 20;
//	
//}
//
//void UInGameUIWidget::TimerRun()
//{
//	// 게임오버 타이머 진행
//
//	FText RestGameTimeText = FText::AsNumber(RestGameTime);
//	GameTimeText->SetText(RestGameTimeText);
//	RestGameTime--;
//	
//
//}

void UInGameUIWidget::OpenInGameUI()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		TinoController->OpenInGameUI();
		TinoController->SetShowMouseCursor(true);
	}
}

void UInGameUIWidget::CloseInGameUI()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
	{
		TinoController->RemoveDialogUI();
		TinoController->SetShowMouseCursor(false);
	}
}

void UInGameUIWidget::ChangeLobbyUI()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	{
		Network::GetNetwork()->bIsConnected = false;
		Network::GetNetwork()->bLevelOpenTriggerEnabled = true;
		UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"));
	}
	//if (!!TinoController)
	//	TinoController->ChangeMenuWidget(TinoController->GetLobbyWidgetClass());
}

void UInGameUIWidget::LevelStartCountdown()
{
	// 레벨 시작시 카운트다운을 위해 이 함수 호출
	InGameTimer = Cast<UInGameTimerWidget>(CreateWidget(GetWorld(), InGameTimerClass));
	if (!!InGameTimer)
	{
		InGameTimer->TimerStart(ETimerType::ETT_LevelStart);
	}
}

void UInGameUIWidget::LevelStart()
{
	// 카운트다운이 끝나고 화면에 go를 띄우는 동시에 레벨 시작
	// 타이머 안보이도록 숨김
	StartCountDownText->SetRenderOpacity(0.0);

}

void UInGameUIWidget::LevelClearCountdown()
{
	// 누군가 결승선을 통과했을 때 레벨을 끝내기까지 남은시간 카운트다운
	if (!!InGameTimer)
	{
		InGameTimer->TimerStart(ETimerType::ETT_LevelClear);
	}
}

void UInGameUIWidget::TimerTextChange(int RestGameTime, ETimerType Type)
{
	if (Type == ETimerType::ETT_LevelClear)
	{
		FText RestGameTimeText = FText::AsNumber(RestGameTime);
		GameTimeText->SetText(RestGameTimeText);
	}
	else if (Type == ETimerType::ETT_LevelStart)
	{
		FText StartCountDown = FText::AsNumber(RestGameTime);
		StartCountDownText->SetText(StartCountDown);
	}
}

