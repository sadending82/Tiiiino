// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Notify/ANS_Dive.h"
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
		TinoCharacter->Jump();
		TinoCharacter->DisableInputMode();
		TinoCharacter->SetMaxTumbleTime(0.3f);
		TinoCharacter->GetCharacterMovement()->MaxWalkSpeed = 0;
	}
	

}

void UANS_Dive::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		/*float DiveSpeed = TinoCharacter->GetVelocity().Size2D();
		if (DiveSpeed <= 0.f) DiveSpeed = MinDiveSpeed;*/
		FVector NewLocation = TinoCharacter->GetActorLocation()
			+ TinoCharacter->GetActorForwardVector() * DiveSpeed * FrameDeltaTime;
		TinoCharacter->SetActorLocation(NewLocation);
	}
	
}

void UANS_Dive::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	auto TinoCharacter = Cast<ATinoCharacter>(MeshComp->GetOwner());
	if (TinoCharacter)
	{
		TinoCharacter->EnableInputMode();
		TinoCharacter->SetMaxTumbleTime(0.5f);
		TinoCharacter->GetCharacterMovement()->MaxWalkSpeed = 400;

	}
}
