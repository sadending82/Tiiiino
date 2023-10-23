// Fill out your copyright notice in the Description page of Project Settings.

#include "Sound/SoundManager.h"
#include "Global.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundCue.h"

ASoundManager* ASoundManager::SoundManagerInstance = nullptr;

ASoundManager::ASoundManager()
	:BGMVolume(0.1f),
	SFXVolume(0.3f)
{
	ASoundManager::SoundManagerInstance = this;
	UHelpers::CreateComponent(this, &SceneComponent, "SceneComponent", GetRootComponent());
	UHelpers::CreateComponent(this, &MainAudio, "MainAudio", SceneComponent);
	UHelpers::CreateComponent(this, &SFXAudio, "SFXAudio", SceneComponent);
	UHelpers::CreateActorComponent(this, &AttenuationSettings, "AttenuationSettings");
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
		
		SFXAudio->SetSound(SFXSoundMap[Type]);
		SFXAudio->Play();
	}
}

void ASoundManager::PlaySFXAtLocation(AActor* PlayActor, ESFXType Type, FVector Location, USoundCue* Sound)
{
	if (SoundManagerInstance && !SFXSoundMap.IsEmpty())
	{
		if (SFXSoundMap.Contains(Type) == false)
		{
			CLog::Log("This Sound doesn't exist");
			return;
		}

		//재생할 액터에 채널이 달려있나 확인
		if (SFXChannelMap.Contains(Type) == false)
			AttachSFXChannel(PlayActor, Type);
		//지정된 사운드가 있으면 그걸로 교체후 재생
		if (!!Sound)
			SFXChannelMap[Type]->SetSound(Sound);

		SFXChannelMap[Type]->Play();
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

void ASoundManager::AttachSFXChannel(AActor* AttachActor, ESFXType Type)
{
	FName Name = FName( "SFxChannel" + StaticCast<int32>(Type));
	UAudioComponent* SFXChannel = NewObject<UAudioComponent>(AttachActor, UAudioComponent::StaticClass(), Name);
	
	if (SFXChannelMap.Contains(Type) == false)
	{
		SFXChannelMap.Add(Type, SFXChannel);
		SFXChannel->SetSound(SFXSoundMap[Type]);
		SFXChannel->AttenuationSettings = AttenuationSettings;
		SFXChannel->RegisterComponent();
		SFXChannel->AttachToComponent(AttachActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	else
		CLog::Log("This SFXType already exist");
}
