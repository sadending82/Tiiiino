// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ClientGameMode.generated.h"

enum class EBGMType : uint8;
class UDataTable;
struct FItemData;

UCLASS(minimalapi)
class AClientGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AClientGameMode();
	
public:

	FItemData* GetItemData(const int64& ItemCode) const;
	FItemData* GetShopProductData(const int64& ItemCode) const;

	void SetItemDataTable();
	void SetShopDataTable();

	void GetShopProductTable(TArray<int>& f_out);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//모든 아이템 데이터 집합 -> 김혁동 : 이 아니랍니다 ㅠㅠ
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* ItemData;

	//상점에서 팔 아이템 데이터들의 집합 -> CBT 쿠폰 보상 아이템 같은건 상점에서 팔면 안됨.
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* ShopData;

	UPROPERTY(EditDefaultsOnly, Category = "Class")
	TSubclassOf<class ASoundManager> SoundMangerClass;

	UPROPERTY(EditDefaultsOnly, Category = "LevelName")
	TMap<FString, EBGMType> ConvertBGMTypeMap;

	
};



