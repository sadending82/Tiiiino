// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUI/RankWidget.h"
#include "Components/CanvasPanelSlot.h"
void URankWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

TArray<float>& URankWidget::GetCharacterDistanceData()
{
	CharacterDistanceData.Empty();

	for (auto& data : RankData)
	{
		CharacterDistanceData.Add(data.first);
	}

	return CharacterDistanceData;
}

bool URankWidget::IsSpectatorModeOn()
{
	auto network = Network::GetNetwork();
	bool tmpSpactateMode = 0;
	if(network->mMyCharacter)
		tmpSpactateMode = network->mMyCharacter->bIsSpactateModeEnabled;
	return tmpSpactateMode;
	 
}

void URankWidget::CalculateNewRank()
{
	auto network = Network::GetNetwork();
	std::sort(RankData.begin(), RankData.end(), [](const auto& lhs, const auto& rhs) {
		return lhs.first > rhs.first;
		});

	int cnt = 1;

	for (auto& data : RankData)
	{
		if (data.second == network->mMyCharacter)
		{
			sCurrentRank = cnt;
			break;
		}
		cnt++;
	}

}

void URankWidget::GetDataOfCharacters()
{

	if (!bIsLevelLoaded) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoalArea::StaticClass(), FoundActors);
		CLog::Log("Found Actors...");
		if (FoundActors.IsEmpty()) {
			return;
		}

		if (FoundActors.Num() > 1) {
			return;
		}

		EndPosition = FoundActors[0]->GetActorLocation();
		fTrackDistance = EndPosition.Y - StartPosition.Y;
		bIsLevelLoaded = true;
	}


	// 10/28 김혁동
	// 구현이 자료구조 적으로 굉장히 좋지 않긴 하지만
	// 나중에 개선할 예정
	// 그치만 8명 밖에 없는데 좀 안좋아도 되지 않을까...
	auto network = Network::GetNetwork();

	// 이 녀석은 순위를 매기기 위한 녀석이기 때문에 매번 클리어 해준다.
	PtrBasedRankData.clear();
	RankData.clear();

	for (int i = 0; i < 8; ++i)
	{
		if (network->mOtherCharacter[i] == nullptr) {
			continue;
		}
		ATinoCharacter* curCharacter = network->mOtherCharacter[i];

		CLog::Log(curCharacter->GetName());

		// 현재 진행한 정도를 총 거리로 나누면 어느정도 진행했는지 나온다.
		// 현재 진행한 정도는 현재 지점 - 스타트 지점 하면 된다.
		float DistanceData = (curCharacter->GetActorLocation().Y - StartPosition.Y) / fTrackDistance;

		if (DistanceData < 0.f) {
			DistanceData = 0.f;
		}

		if (DistanceData > 1.f) {
			DistanceData = 1.f;
		}

		PtrBasedRankData.insert(std::make_pair(curCharacter, DistanceData));
		CLog::Log(DistanceData);

		RankData.emplace_back(std::make_pair(DistanceData, curCharacter));
		OtherCharacterDistanceData.Add(DistanceData);
	}

	if (network->mMyCharacter != nullptr) {
		ATinoCharacter* curCharacter = network->mMyCharacter;

		float DistanceData = (curCharacter->GetActorLocation().Y - StartPosition.Y) / fTrackDistance;

		if (DistanceData < 0.f) {
			DistanceData = 0.f;
		}

		if (DistanceData > 1.f) {
			DistanceData = 1.f;
		}

		PtrBasedRankData.insert(std::make_pair(curCharacter, DistanceData));

		RankData.emplace_back(std::make_pair(DistanceData, curCharacter));
		myCharcaterDistanceData = DistanceData;
	}
	else {
		myCharcaterDistanceData = -1.0f;
	}
}

void URankWidget::ChangePositionByDistanceData()
{
	std::vector<UImage*> images;
	
	images.push_back(OtherCharacterImage0);
	images.push_back(OtherCharacterImage1);
	images.push_back(OtherCharacterImage2);
	images.push_back(OtherCharacterImage3);
	images.push_back(OtherCharacterImage4);
	images.push_back(OtherCharacterImage5);
	images.push_back(OtherCharacterImage6);

	TArray<float> distanceData = GetCharacterDistanceData();

	int cnt{0};
	for (int i = 0; i < distanceData.Num(); ++i)
	{
		if (i == sCurrentRank - 1)
		{
			UCanvasPanelSlot* canvasPanel = UWidgetLayoutLibrary::SlotAsCanvasSlot(MyCharacterImage);
			if (!IsSpectatorModeOn())
			{
				canvasPanel->SetPosition(FVector2D(1280.f * distanceData[sCurrentRank - 1], 0.0f));
			}
			else {
				canvasPanel->SetPosition(FVector2D(1280.f, 0.0f));
			}
			continue;
		}
		UCanvasPanelSlot* canvasPanel = UWidgetLayoutLibrary::SlotAsCanvasSlot(images[cnt]);
		canvasPanel->SetPosition(FVector2D(1280.f * distanceData[i], 0.0f));
		CLog::Log("Set");
		cnt++;
		CLog::Log(cnt);
	}

	for (; cnt < images.size(); ++cnt)
	{	
		UCanvasPanelSlot* canvasPanel = UWidgetLayoutLibrary::SlotAsCanvasSlot(images[cnt]);
		canvasPanel->SetPosition(FVector2D(-1920.f, 0.0f));
		CLog::Log("Remove");
		CLog::Log(cnt);
	}

}

void URankWidget::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);

	// 캐릭터 데이터 모으기
	GetDataOfCharacters();

	// 실제 랭크 가져오기
	CalculateNewRank();

	// 랭크 데이터를 바탕으로 위치 바꿔주기
	ChangePositionByDistanceData();
}
