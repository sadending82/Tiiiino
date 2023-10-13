// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_PlayFootStep.h"
#include "Actor/Character/TinoCharacter.h"
#include "Sound/SoundManager.h"
#include "Global.h"


FString UAN_PlayFootStep::GetNotifyName_Implementation() const
{
	return "PlayFootStep";
}

void UAN_PlayFootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter  && TinoCharacter->bIsSpactateModeEnabled == false)
	{
		if(TinoCharacter->bIsPlaySelf && TinoCharacter->bIsControlledPlayer)
			ASoundManager::GetSoundManager()->PlaySFXAtLocation(FootstepCue, TinoCharacter->GetActorLocation());
		else if (TinoCharacter->bIsPlaySelf == false)
			ASoundManager::GetSoundManager()->PlaySFXAtLocation(FootstepCue, TinoCharacter->GetActorLocation());

	}
}
