// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/ANS_Grab.h"
#include "Actor/Character/TinoCharacter.h"

FString UANS_Grab::GetNotifyName_Implementation() const
{
	return "Grab";
}

void UANS_Grab::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		TinoCharacter->GrabBegin();
	}
}

void UANS_Grab::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	return;
	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		TinoCharacter->SetMovementState(EMovementState::EMS_Grabbing);
	}
}
