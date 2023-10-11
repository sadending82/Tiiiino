// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PlayFootStep.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API UAN_PlayFootStep : public UAnimNotify
{
	GENERATED_BODY()

public:

	FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "AnimNotify")
	class USoundCue* FootstepCue;
};
