#pragma once

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

USTRUCT()
struct FItemTextData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ToolTip = "아이템 이름"))
		FText Name;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "아이템 설명 마우스 가져다대면 띄우기 가능"))
		FText Description;
};

USTRUCT()
struct FItemNumericData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ToolTip = "구매 조건"))
		float PurchaseCondition;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "구매 제한 여부"))
		bool bIsPurchasable;
};

USTRUCT()
struct FItemAssetData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ToolTip = "아이템 아이콘(인벤토리UI)"))
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "사용할 블루프린트 클래스"))
		TSubclassOf<class AAccessoryItem> BPClass;
};


USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemData() :
	ItemCode(),
	EquipType(),
	TextData(),
	NumericData(),
	AssetData(),
	SellValue()
	{

	}

	UPROPERTY(EditAnywhere, Category = "ItemData")
		int64 ItemCode;

	UPROPERTY(EditAnywhere, Category = "ItemData")

		EEquipType EquipType;

	UPROPERTY(EditAnywhere, Category = "ItemData")
		FItemTextData TextData;
	UPROPERTY(EditAnywhere, Category = "ItemData")
		FItemNumericData NumericData;
	UPROPERTY(EditAnywhere, Category = "ItemData")
		FItemAssetData AssetData;

	UPROPERTY(EditAnywhere, Category = "ItemData")
		int32 SellValue;


	bool operator==(const FItemData& OtherData) const
	{
		return this->ItemCode == OtherData.ItemCode;
	}
};