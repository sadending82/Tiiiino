#include "GameDataManager.h"

bool GameDataManager::LoadShopData()
{
	try {
		tinyxml2::XMLDocument doc;

		doc.LoadFile(SHOPDATAFILE);

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pProductList = pRoot->FirstChildElement("productlist");

		for (tinyxml2::XMLElement* pProduct = pProductList->FirstChildElement("product")
			; pProduct != NULL; pProduct = pProduct->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pProduct->FirstChildElement("code")->GetText());
			item.name = pProduct->FirstChildElement("name")->GetText();
			item.price = stoi(pProduct->FirstChildElement("price")->GetText());
			item.cutline = stoi(pProduct->FirstChildElement("cutline")->GetText());

			ShopProductsList[item.itemCode] = item;
		}
		return true;
	}
	catch(exception ex){ 
		DEBUGMSGNOPARAM("Load Shop Data Failed\n");
		return false;
	}
}

bool GameDataManager::LoadItemData()
{
	try {
		tinyxml2::XMLDocument doc;

		doc.LoadFile(ITEMDATAFILE);

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pItemList = pRoot->FirstChildElement("itemlist");

		for (tinyxml2::XMLElement* pItem = pItemList->FirstChildElement("item")
			; pItem != NULL; pItem = pItem->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pItem->FirstChildElement("code")->GetText());
			item.name = pItem->FirstChildElement("name")->GetText();
			item.price = stoi(pItem->FirstChildElement("price")->GetText());

			ItemList[item.itemCode] = item;
		}
		return true;
	}
	catch (exception ex) {
		DEBUGMSGNOPARAM("Load Item Data Failed\n");
		return false;
	}
}

bool GameDataManager::LoadCouponData()
{
	try {
		tinyxml2::XMLDocument doc;

		doc.LoadFile(COUPONDATAFILE);

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pCouponList = pRoot->FirstChildElement("couponlist");

		for (tinyxml2::XMLElement* pCoupon = pCouponList->FirstChildElement("coupon")
			; pCoupon != NULL; pCoupon = pCoupon->NextSiblingElement()) {

			string couponCode = pCoupon->FirstChildElement("couponCode")->GetText();
			int itemCode = stoi(pCoupon->FirstChildElement("itemCode")->GetText());

			CouponList[couponCode] = itemCode;
		}
		return true;
	}
	catch (exception ex) {
		DEBUGMSGNOPARAM("Load Coupon Data Failed\n");
		return false;
	}
}