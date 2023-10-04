// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GoalArea.h"
#include "GameFramework/Character.h"
#include "Global.h"
#include "Network/Network.h"

// Sets default values
AGoalArea::AGoalArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UHelpers::CreateComponent<UBoxComponent>(this, &OverlapBox, "OverlapBox", RootComponent);
}

// Called when the game starts or when spawned
void AGoalArea::BeginPlay()
{
	Super::BeginPlay();

	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AGoalArea::BoxBeginOverlap);
	
}

// Called every frame
void AGoalArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoalArea::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* OverlapCharacter = Cast<ACharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
		AController* Controller = OverlapCharacter->GetController();
		if (!!Controller)
		{
			if (Controller->IsPlayerController())
			{
				// �ڽ� ������Ʈ�� �� ĳ���Ͱ� ��Ҵ�!
				send_goal_packet(Network::GetNetwork()->s_socket);

				// ĳ���͸� �Ⱥ��̰� �ϰ� �ݸ����� off
				PlayerDisable(OverlapCharacter);
				// UI Ÿ�̸� ������ UI ��� ����� �ΰ��� UI�� �ϼ��Ǵ� ��� �߰�
				

				// ���� ��� ��ȯ...? -> �� ���� ��

			}
		}
		else
			CLog::Log("Controller is nullptr");
	}
}

void AGoalArea::PlayerDisable(ACharacter* Character)
{
	Character->SetActorEnableCollision(false);
	Character->SetActorHiddenInGame(true);
}

