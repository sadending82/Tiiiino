// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/FinishGameUIWidget.h"
#include "Components/TextBlock.h"

void UFinishGameUIWidget::NativePreConstruct()
{

}

void UFinishGameUIWidget::NativeDestruct()
{
}

void UFinishGameUIWidget::NativeConstruct()
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
	MapNameArray[2] = L"티노와 함께 춤을";
	MapNameArray[3] = L"티노 L3라이센스 시험장";
	MapNameArray[4] = L"선데이 티노";
	MapNameArray[5] = L"스페이스 티노 2023";
	MapNameArray[6] = L"티노 풀 회전!";
	MapNameArray[7] = L"티노의 습격";


	// 맵 이름 입력
	CurrentStage = MapNameArray[level];	// 맵 이름 입력
	FText CurrentStageText = FText::FromString(CurrentStage);
	StageNameText->SetText(CurrentStageText);


	// 이 판 등수 입력
	if (rank <= 0)
	{
		FText RetireText = FText::FromString("Retire");
		RankNameText->SetText(RetireText);
	}
	else
	{
		CurrentRank = rank;	// 등수 입력
		FText CurrentRankText = FText::AsNumber(CurrentRank);
		RankNameText->SetText(CurrentRankText);
	}


	// 획득한 학점 입력
	GainGrade = grade;	// 등수 입력
	FText GainGradeText = FText::AsNumber(GainGrade);
	GradeNameText->SetText(GainGradeText);


	// 획득한 포인트 입력
	GainPoint = point;	// 등수 입력
	FText GainPointText = FText::AsNumber(GainPoint);
	PointNameText->SetText(GainPointText);

}

