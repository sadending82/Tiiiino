#pragma once
#include "stdafx.h"
#include "tinyxml2.h"
#include <map>

constexpr int MAXITEM = 64;

#define SHOPDATAFILE "../../GameData/ShopData.xml"
#define ITEMDATAFILE "../../GameData/ItemData.xml"
#define COUPONDATAFILE "../../GameData/CouponData.xml"

struct Item
{
	int		itemCode;
	string	name;
	int		price;
};

class GameDataManager
{
public:
	void			LoadShopData();
	void			LoadItemData();
	void			LoadCouponData();

	map<int, Item>	GetShopProductsList() { return ShopProductsList; }
	map<int, Item>	GetItemList() { return ItemList; }
	Item			GetShopProductInfo(int itemCode) { return ShopProductsList[itemCode]; }
	Item			GetItemInfo(int itemCode) { return ItemList[itemCode]; }

private:
	// key = itemCode
	map<int, Item> ShopProductsList;
	map<int, Item> ItemList;

	// key = coupon number / value = itemcode
	map<string, int> CouponList;
};