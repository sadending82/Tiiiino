#pragma once

#include "GameDataManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>


bool GameDataManager::CheckDataFile()
{
	std::string sourceFilePath{ TCHAR_TO_ANSI(*FPaths::GameSourceDir()) };
	std::string launchFilePath{ TCHAR_TO_ANSI(*FPaths::LaunchDir()) };
	sourceFilePath += EXTERNAL_SHOPDATAFILE;

	SHOPDATAFILE = launchFilePath + INTERNAL_SHOPDATAFILE;
	ITEMDATAFILE = launchFilePath + INTERNAL_ITEMDATAFILE;

	if (GetFileAttributesA(sourceFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
	
		std::ifstream source(sourceFilePath, std::ios::binary);
		std::stringstream ss;
		if (source)	ss << source.rdbuf();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), sourceFilePath.c_str());
		source.close();
		std::filesystem::path dirPath = (launchFilePath + INTERNAL_DATADIR);
		if (!std::filesystem::is_directory(dirPath)) std::filesystem::create_directories(dirPath);
		std::ofstream destination(SHOPDATAFILE,std::ios::binary);
		if (destination) destination << ss.str();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), SHOPDATAFILE.c_str());
		destination.close();
	}


	sourceFilePath = TCHAR_TO_ANSI(*FPaths::GameSourceDir());
	sourceFilePath += EXTERNAL_ITEMDATAFILE;

	if (GetFileAttributesA(sourceFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {

		std::ifstream source(sourceFilePath, std::ios::binary);
		std::stringstream ss;
		if (source)	ss << source.rdbuf();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), sourceFilePath.c_str());
		source.close();
		std::filesystem::path dirPath = launchFilePath + INTERNAL_DATADIR;
		if (!std::filesystem::is_directory(dirPath)) std::filesystem::create_directories(dirPath);
		std::ofstream destination(ITEMDATAFILE, std::ios::binary);
		if (destination) destination << ss.str();
		else UE_LOG(LogTemp, Error, TEXT("%s doesn't Exist"), ITEMDATAFILE.c_str());
		destination.close();
	}


	return true;
}

bool GameDataManager::LoadShopData()
{
	try {
		tinyxml2::XMLDocument doc;

		if (doc.LoadFile(SHOPDATAFILE.c_str()) != tinyxml2::XMLError::XML_SUCCESS) {
			UE_LOG(LogTemp, Error, TEXT("%s Load Failed"), SHOPDATAFILE.c_str());
			return false;
		}

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pProductList = pRoot->FirstChildElement("productlist");

		for (tinyxml2::XMLElement* pProduct = pProductList->FirstChildElement("product")
			; pProduct != NULL; pProduct = pProduct->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pProduct->FirstChildElement("code")->GetText());
			item.price = stoi(pProduct->FirstChildElement("price")->GetText());
			item.cutline = stof(pProduct->FirstChildElement("cutline")->GetText());
			item.assetName = pProduct->FirstChildElement("assetName")->GetText();
			const char* uft8Name = pProduct->FirstChildElement("name")->GetText();
			const char* uft8text = pProduct->FirstChildElement("text")->GetText();
			wchar_t unicodeName[256] = { 0, };
			wchar_t unicodeText[256] = { 0, };
			int nLen = MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), unicodeName, nLen);
			nLen = MultiByteToWideChar(CP_UTF8, 0, uft8text, strlen(uft8text), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, uft8text, strlen(uft8text), unicodeText, nLen);
			item.name = wstring(unicodeName).c_str();
			item.text = wstring(unicodeText).c_str();
			ShopProductsList[item.itemCode] = item;
		}
		UE_LOG(LogTemp, Log, TEXT("Shop Data Load Succeed"));
		return true;
	}
	catch(exception ex){ 
		return false;
	}
}

bool GameDataManager::LoadItemData()
{
	try {
		tinyxml2::XMLDocument doc;

		if (doc.LoadFile(ITEMDATAFILE.c_str()) != tinyxml2::XMLError::XML_SUCCESS) {
			UE_LOG(LogTemp, Error, TEXT("%s Load Failed"), ITEMDATAFILE.c_str());
			return false;
		}

		tinyxml2::XMLElement* pRoot = doc.RootElement();
		tinyxml2::XMLElement* pItemList = pRoot->FirstChildElement("itemlist");

		for (tinyxml2::XMLElement* pItem = pItemList->FirstChildElement("item")
			; pItem != NULL; pItem = pItem->NextSiblingElement()) {

			Item item;
			item.itemCode = stoi(pItem->FirstChildElement("code")->GetText());
			item.price = stoi(pItem->FirstChildElement("price")->GetText());
			item.assetName = pItem->FirstChildElement("assetName")->GetText(); 
			const char* uft8Name = pItem->FirstChildElement("name")->GetText();
			const char* uft8text = pItem->FirstChildElement("text")->GetText();
			wchar_t unicodeName[256] = { 0, };
			wchar_t unicodeText[256] = { 0, };
			int nLen = MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, uft8Name, strlen(uft8Name), unicodeName, nLen);
			nLen = MultiByteToWideChar(CP_UTF8, 0, uft8text, strlen(uft8text), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, uft8text, strlen(uft8text), unicodeText, nLen);
			item.name = wstring(unicodeName).c_str();
			item.text = wstring(unicodeText).c_str();
			ItemList[item.itemCode] = item;
		}
		UE_LOG(LogTemp, Log, TEXT("Item Data Load Succeed"));
		return true;
	}
	catch (exception ex) {
		return false;
	}
}