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
	// ���� ����
	

}

void UInGameUIWidget::LevelFailed()
{
	// ���� ����

}

void UInGameUIWidget::LevelClearCheck()
{
	// ���� ���� ���� üũ
	// ������ bLevelClearCheck = true
	// ���н� bLevelClearCheck = false
	
}

void UInGameUIWidget::TimerStart()
{

	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	auto TinoCharacter = TinoController->GetPawn<ATinoCharacter>();
	if (TinoCharacter)
	{
		// ATinoCharacter �ν��Ͻ��� ����Ͽ� TimerStart �Լ��� ȣ��
		TinoCharacter->TimerStart();
	}
}

void UInGameUIWidget::TimerEnd()
{
	// Timer end
	// Reset RestGameTime
	RestGameTime = 20;
	
}

void UInGameUIWidget::TimerRun()
{
	// ���ӿ��� Ÿ�̸� ����

	FText RestGameTimeText = FText::AsNumber(RestGameTime);
	GameTimeText->SetText(RestGameTimeText);
	RestGameTime--;
	

}

void UInGameUIWidget::OpenInGameUI()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
		TinoController->OpenInGameUI();
}

void UInGameUIWidget::CloseInGameUI()
{
	auto TinoController = Cast<ATinoController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!!TinoController)
		TinoController->RemoveDialogUI();
}

