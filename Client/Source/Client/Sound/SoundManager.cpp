// Fill out your copyright notice in the Description page of Project Settings.

#include "Sound/SoundManager.h"
#include "Global.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

ASoundManager* ASoundManager::SoundManagerInstance = nullptr;

ASoundManager::ASoundManager()
	:BGMVolume(0.1f),
	SFXVolume(0.3f)
{
	ASoundManager::SoundManagerInstance = this;
	UHelpers::CreateComponent(this, &MainAudio, "MainAudio",GetRootComponent());
	UHelpers::CreateComponent(this, &SFXAudio, "SFXAudio", GetRootComponent());

	MainAudio->SetVolumeMultiplier(BGMVolume);
	SFXAudio->SetVolumeMultiplier(SFXVolume);

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

void ASoundManager::PlaySFX2D(ESFXType Type)
{
	if (SoundManagerInstance && !SFXSoundMap.IsEmpty())
	{
		if (SFXSoundMap.Contains(Type) == false) return;
		UGameplayStatics::PlaySound2D(GetWorld(), SFXSoundMap[Type], SFXVolume);
		//
		//SFXSoundMap[Type]->PlaySound
		//SFXAudio->SetSound();
		//SFXAudio->
	}
}

void ASoundManager::PlaySFXAtLocation(ESFXType Type, FVector Location)
{
	if (SoundManagerInstance && !SFXSoundMap.IsEmpty())
	{
		if (SFXSoundMap.Contains(Type) == false) return;
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SFXSoundMap[Type], Location,SFXVolume);
		//
		//SFXSoundMap[Type]->PlaySound
		//SFXAudio->SetSound();
		//SFXAudio->
	}
}

void ASoundManager::PlaySFXAtLocation(USoundCue* Sound, FVector Location)
{
	if (SoundManagerInstance && Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, SFXVolume);
		//SFXAudio->SetSound(Sound);
		//SFXAudio->Play();
	}
}

void ASoundManager::SetBGMVolume(const float Volume)
{
	MainAudio->SetVolumeMultiplier(BGMVolume);
}

void ASoundManager::SetSFXVolume(const float Volume)
{
	SFXAudio->SetVolumeMultiplier(SFXVolume);
}
