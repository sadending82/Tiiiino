// Fill out your copyright notice in the Description page of Project Settings.
#include "Notify/ANS_Dive.h"
#include "Actor/Character/TinoCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Global.h"

FString UANS_Dive::GetNotifyName_Implementation() const
{
	return "Dive";
}

void UANS_Dive::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp,Animation,TotalDuration,EventReference);
	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		TinoCharacter->DiveBegin();

		//Before Launch Saving Data
		OriginalTumbleTime = TinoCharacter->GetMaxTumbleTime();
		TinoCharacter->SetMaxTumbleTime(ChangeTumbleTime);

		//Player Launch
		FVector forwardVector = TinoCharacter->GetActorForwardVector();
		TinoCharacter->LaunchCharacter(FVector(forwardVector.X* DiveSpeed,forwardVector.Y*DiveSpeed,  DiveSpeedZ),true,true);
	}

}

void UANS_Dive::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		//Restore Saving Data
		TinoCharacter->SetMaxTumbleTime(OriginalTumbleTime);
		TinoCharacter->SetMovementState(EMovementState::EMS_Normal);
	}
}
