// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/ANS_Tumble.h"
#include "Actor/Character/TinoCharacter.h"

FString UANS_Tumble::GetNotifyName_Implementation() const
{
	return "Tumble";
}

void UANS_Tumble::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		TinoCharacter->SetMovementState(EMovementState::EMS_Tumbled);
	}
}

void UANS_Tumble::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		TinoCharacter->DiveEnd();
		TinoCharacter->SetMovementState(EMovementState::EMS_Normal);
	}
}
