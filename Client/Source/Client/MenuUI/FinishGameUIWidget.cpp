﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/FinishGameUIWidget.h"
#include "Components/EditableText.h"

void UFinishGameUIWidget::NativePreConstruct()
{
}

void UFinishGameUIWidget::NativeDestruct()
{
}

void UFinishGameUIWidget::EnterLobby()
{
	// EnterLobbyButton Onclicked
	// 로비로 돌아가기

}

void UFinishGameUIWidget::ShowResult()
{
	// 맵 이름 입력
	CurrentStage = TEXT("맵 이름");	// 맵 이름 입력
	FText CurrentStageText = FText::FromString(CurrentStage);
	EditSTAGEText->SetText(CurrentStageText);


	// 이 판 등수 입력
	CurrentRank = 1;	// 등수 입력
	FText CurrentRankText = FText::AsNumber(CurrentRank);
	EditRANKText->SetText(CurrentRankText);


	// 획득한 학점 입력
	GainGrade = 10;	// 등수 입력
	FText GainGradeText = FText::AsNumber(GainGrade);
	EditGradeText->SetText(GainGradeText);


	// 획득한 포인트 입력
	GainPoint = 100;	// 등수 입력
	FText GainPointText = FText::AsNumber(GainPoint);
	EditPointText->SetText(GainPointText);

}

