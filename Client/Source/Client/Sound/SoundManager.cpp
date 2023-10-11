// Fill out your copyright notice in the Description page of Project Settings.

#include "Sound/SoundManager.h"
#include "Global.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

ASoundManager* ASoundManager::SoundManagerInstance = nullptr;

ASoundManager::ASoundManager()
{
	ASoundManager::SoundManagerInstance = this;
	UHelpers::CreateComponent(this, &MainAudio, "MainAudio",GetRootComponent());
	UHelpers::CreateComponent(this, &SFXAudio, "SFXAudio", GetRootComponent());

}

void ASoundManager::SetBGM(EBGMType Type)
{
	if (BGMSoundMap.IsEmpty() == false)
	{
		if (BGMSoundMap[Type] == nullptr) return;
		MainBGM = BGMSoundMap[Type];

	}
}

void ASoundManager::PlayBGM()
{
	if (SoundManagerInstance && MainBGM)
	{
		MainAudio->SetSound(MainBGM);
		MainAudio->Play();
	}

}

void ASoundManager::PlaySFX(ESFXType Type)
{
	if (SoundManagerInstance && !SFXSoundMap.IsEmpty())
	{
		if (SFXSoundMap[Type] == nullptr) return;
		SFXAudio->SetSound(SFXSoundMap[Type]);
		SFXAudio->Play();
	}
}

void ASoundManager::PlaySFX(USoundCue* Sound)
{
	if (SoundManagerInstance && Sound)
	{
		SFXAudio->SetSound(Sound);
		SFXAudio->Play();
	}
}
