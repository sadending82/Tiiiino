#pragma once
#include "tinyxml2.h"
#include <map>
#include <string>
#include <Windows.h>

#include "../Global.h"

using namespace std;

constexpr int MAXITEM = 64;

#define EXTERNAL_SHOPDATAFILE "../../GameData/ShopData.xml"
#define EXTERNAL_ITEMDATAFILE "../../GameData/ItemData.xml"

#define INTERNAL_DATADIR "/Data"
#define INTERNAL_SHOPDATAFILE "/Data/ShopData.xml"
#define INTERNAL_ITEMDATAFILE "/Data/ItemData.xml"

struct Item
{
	int		itemCode;
	FString	name;
	int		price;
	double  cutline = 0.0;
	FString assetName;
	FString text;
};

class GameDataManager
{
public:
	bool			CheckDataFile();

	bool			LoadShopData();
	bool			LoadItemData();

	map<int, Item>	GetShopProductsList() { return ShopProductsList; }
	map<int, Item>	GetItemList() { return ItemList; }
	Item			GetShopProductInfo(int itemCode) { return ShopProductsList[itemCode]; }
	Item			GetItemInfo(int itemCode) { return ItemList[itemCode]; }

private:
	// key = itemCode
	map<int, Item> ShopProductsList;
	map<int, Item> ItemList;

	string		SHOPDATAFILE;
	string		ITEMDATAFILE;
};