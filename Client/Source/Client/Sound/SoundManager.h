// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"

UENUM(BlueprintType)
enum class EBGMType : uint8
{
	EBGMType_None UMETA(DisplayName = "None"),
	EBGMType_Lobby UMETA(DisplayName = "Lobby"),
	EBGMType_Race UMETA(DisplayName = "InGame")
};

UENUM(BlueprintType)
enum class ESFXType : uint8
{
	ESFXType_None UMETA(DisplayName = "None"),
	ESFXType_Footstep UMETA(DisplayName = "Footstep"),
	ESFXType_Click UMETA(DisplayName = "Click"),
	ESFXType_Respawn UMETA(DisplayName = "Respawn"),
	ESFXType_RaceCountDown UMETA(DisplayName = "RaceCountDown"),
	ESFXType_RaceStart UMETA(DisplayName = "RaceStart"),
	ESFXType_ObstacleAccel UMETA(DisplayName = "ObstacleAccel")
};

UCLASS()
class CLIENT_API ASoundManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASoundManager();
public:
	UFUNCTION(BlueprintPure)
		static ASoundManager* GetSoundManager() { return SoundManagerInstance; }

	UFUNCTION(BlueprintCallable)
		void SetBGM(EBGMType Type);
	UFUNCTION(BlueprintCallable)
		void PlayBGM();

	UFUNCTION(BlueprintCallable)
		void PlaySFX(ESFXType Type, FVector Location = FVector::ZeroVector);
	//UFUNCTION(BlueprintCallable)
	void PlaySFX(class USoundCue* Sound, FVector Location = FVector::ZeroVector);

	UFUNCTION(BlueprintPure)
	FORCEINLINE	 float GetBGMVolume() const { return BGMVolume; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE	 float GetSFXVolume() const { return SFXVolume; }
	UFUNCTION(BlueprintCallable)
		void SetBGMVolume(const float Volume);
	UFUNCTION(BlueprintCallable)
		void SetSFXVolume(const float Volume);



private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UAudioComponent* MainAudio;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		class UAudioComponent* SFXAudio;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		class USoundCue* MainBGM;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		float BGMVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		float SFXVolume;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		TMap<EBGMType, USoundCue*> BGMSoundMap;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		TMap<ESFXType, USoundCue*> SFXSoundMap;

	static ASoundManager* SoundManagerInstance;
};
