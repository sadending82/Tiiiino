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
	RankData.clear();

	for (int i = 0; i < 8; ++i)
	{
		if (network->mOtherCharacter[i] == nullptr) {
			continue;
		}
		ATinoCharacter* curCharacter = network->mOtherCharacter[i];

		// 현재 진행한 정도를 총 거리로 나누면 어느정도 진행했는지 나온다.
		// 현재 진행한 정도는 현재 지점 - 스타트 지점 하면 된다.
		float DistanceData = (curCharacter->GetActorLocation().Y - StartPosition.Y) / fTrackDistance;

		if (DistanceData < 0.f) {
			DistanceData = 0.f;
		}

		if (DistanceData > 1.f) {
			DistanceData = 1.f;
		}

		auto findIt = PtrBasedRankData.find(curCharacter);

		// Map 내부에 이미 들어가 있나?
		if (findIt == PtrBasedRankData.end()) {
			// Map 내부에 없으면 추가해 줘야 한다
			PtrBasedRankData.insert(std::make_pair(curCharacter, DistanceData));
		}
		else {
			// Map 내부에 있으면 값만 갱신해 주면 된다.
			(*findIt).second = DistanceData;
		}
		
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

		auto findIt = PtrBasedRankData.find(curCharacter);

		// Map 내부에 이미 들어가 있나?
		if (findIt == PtrBasedRankData.end()) {
			// Map 내부에 없으면 추가해 줘야 한다
			PtrBasedRankData.insert(std::make_pair(curCharacter, DistanceData));
		}
		else {
			// Map 내부에 있으면 값만 갱신해 주면 된다.
			(*findIt).second = DistanceData;
		}

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

	for (int i = 0; i < distanceData.Num(); ++i)
	{
		if (i == sCurrentRank - 1 ) continue;
		UCanvasPanelSlot* canvasPanel = UWidgetLayoutLibrary::SlotAsCanvasSlot(images[i]);
		canvasPanel->SetPosition(FVector2D(1280.f * distanceData[i], 0.0f));
	}
	UCanvasPanelSlot* canvasPanel = UWidgetLayoutLibrary::SlotAsCanvasSlot(MyCharacterImage);
	canvasPanel->SetPosition(FVector2D(1280.f * distanceData[sCurrentRank - 1], 0.0f));
	



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
