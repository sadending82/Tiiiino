// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Obstacles/AcceleratePlate.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"



// Sets default values
AAcceleratePlate::AAcceleratePlate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRootComponent = CreateDefaultSubobject <USceneComponent>("RootSceneComponent");
	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>("MainMesh");

	SceneRootComponent->SetupAttachment(RootComponent);
	MainMesh->SetupAttachment(SceneRootComponent);
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &OverlapMesh, "OverlapMesh", SceneRootComponent);

	OverlapMesh->OnComponentBeginOverlap.AddDynamic(this, &AAcceleratePlate::OnOverlapBegin);
	OverlapMesh->OnComponentEndOverlap.AddDynamic(this, &AAcceleratePlate::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AAcceleratePlate::BeginPlay()
{
	Super::BeginPlay();

}

void AAcceleratePlate::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ACharacter* MyCharacter = Cast<ACharacter>(OtherActor);
	if (MyCharacter != nullptr)
	{
		bIsAccelerationOn = true;
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = AccelSpeed;
	}
}

void AAcceleratePlate::NotifyActorEndOverlap(AActor* OtherActor)
{
	ACharacter* MyCharacter = Cast<ACharacter>(OtherActor);
	if (MyCharacter != nullptr)
	{
		bIsAccelerationOn = false;


		TArray<AActor*> AccelActors;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAcceleratePlate::StaticClass(), AccelActors);

		bool bIsSomeoneAccelOn = false;

		for (auto& actor : AccelActors)
		{
			AAcceleratePlate* AccelActor = Cast<AAcceleratePlate>(actor);

			if (AccelActor != nullptr)
			{
				if (AccelActor == this) continue;

				if (AccelActor->bIsAccelerationOn) {
					bIsSomeoneAccelOn = true;
					break;
				}
			}
		}

		if (!bIsSomeoneAccelOn)
		{
			MyCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		}
	}
}

void AAcceleratePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = Cast<ACharacter>(OtherActor);
	if (MyCharacter != nullptr)
	{
	}
}

void AAcceleratePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AAcceleratePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

