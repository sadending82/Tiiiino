// Fill out your copyright notice in the Description page of Project Settings.


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

	RemoveFromParent();
}

void UFinishGameUIWidget::ShowResult(int level, int rank, double grade, int point)
{

	MapNameArray[0] = L"None";
	MapNameArray[1] = L"티노 동산";
	MapNameArray[2] = L"신비한 티노의 정원";
	MapNameArray[3] = L"무의 정원";
	MapNameArray[4] = L"빙글빙글돌아가는맷돌";
	MapNameArray[5] = L"티노의 우주여행";
	MapNameArray[6] = L"떨어지면 망함";
	MapNameArray[7] = L"몰?루";

	// 맵 이름 입력
	CurrentStage = MapNameArray[level];	// 맵 이름 입력
	FText CurrentStageText = FText::FromString(CurrentStage);
	EditSTAGEText->SetText(CurrentStageText);


	// 이 판 등수 입력
	CurrentRank = rank;	// 등수 입력
	FText CurrentRankText = FText::AsNumber(CurrentRank);
	EditRANKText->SetText(CurrentRankText);


	// 획득한 학점 입력
	GainGrade = grade;	// 등수 입력
	FText GainGradeText = FText::AsNumber(GainGrade);
	EditGradeText->SetText(GainGradeText);


	// 획득한 포인트 입력
	GainPoint = point;	// 등수 입력
	FText GainPointText = FText::AsNumber(GainPoint);
	EditPointText->SetText(GainPointText);

}

