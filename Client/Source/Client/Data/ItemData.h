﻿#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EEquipType : uint8
{
	EEquipType_None UMETA(DisplayName = "None"),
	EEquipType_Back UMETA(DisplayName = "Back"),
	EEquipType_Hand UMETA(DisplayName = "Hand"),
	EEquipType_Face UMETA(DisplayName = "Face"),
	EEquipType_Head UMETA(DisplayName = "Head"),
	EEquipType_DefaultMax UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FItemTextData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "아이템 이름"))
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "아이템 설명 마우스 가져다대면 띄우기 가능"))
		FText Description;
};

USTRUCT(BlueprintType)
struct FItemNumericData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "구매 조건"))
		float PurchaseCondition;
};

USTRUCT(BlueprintType)
struct FItemAssetData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "아이템 아이콘(인벤토리UI)"))
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "사용할 블루프린트 클래스"))
	TSubclassOf<class AAccessoryItem> BPClass;
};


USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemData() :
	ItemCode(-1),
	EquipType(),
	TextData(),
	NumericData(),
	AssetData(),
	SellValue()
	{

	}

	FItemData(int itemCode, EEquipType equipType, FText name, FText description, float numericData, int32 sellValue)
		: ItemCode(itemCode), EquipType(equipType), SellValue(sellValue)
	{
		TextData.Name = name;
		TextData.Description = description;

		NumericData.PurchaseCondition = numericData;
	}

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "ItemData")
		int64 ItemCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")

		EEquipType EquipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
		FItemTextData TextData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
		FItemNumericData NumericData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
		FItemAssetData AssetData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemData")
		int32 SellValue;


	bool operator==(const FItemData& OtherData) const
	{
		return this->ItemCode == OtherData.ItemCode;
	}
};