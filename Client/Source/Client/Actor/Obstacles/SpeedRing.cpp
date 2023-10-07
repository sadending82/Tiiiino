#include "Actor/Obstacles/SpeedRing.h"
//#include "Network/Network.h"
#include "Actor/Character/TinoCharacter.h"
#include "Global.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SpeedRing.h"

// Sets default values
ASpeedRing::ASpeedRing()
	:ChangeSpeed(600.f), DurationTime(3.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UHelpers::CreateComponent<USceneComponent>(this, &SceneComponent, "SceneComponent");
	UHelpers::CreateComponent<UStaticMeshComponent>(this, &Mesh, "Mesh", SceneComponent);
	UHelpers::CreateComponent<UBoxComponent>(this, &OverlapBox, "OverlapBox", SceneComponent);
}

// Called when the game starts or when spawned
void ASpeedRing::BeginPlay()
{
	Super::BeginPlay();
	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ASpeedRing::BoxBeginOverlap);
}

// Called every frame
void ASpeedRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpeedRing::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapCharacter = Cast<ATinoCharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
		auto Controller = OverlapCharacter->GetController();
		if (!!Controller)
		{
			if (Controller->IsPlayerController())
			{
				//���� �ӵ��� �ٲ۴�. �ٸ� �÷��̾�� �������� ������ �ӵ��� move packet�� ������ ��?
				//�ʱ�ӵ������� ������ʵ���
				if(GetWorldTimerManager().IsTimerActive(SpeedRingTImer) == false) 
					OriginalSpeed = OverlapCharacter->GetCharacterMovement()->MaxWalkSpeed;
				else
				{	//Ÿ�̸� �ߺ� ����, �̹������� �����ϰ� �ٽ� ����
					GetWorldTimerManager().ClearTimer(SpeedRingTImer);
				}
				OverlapCharacter->GetCharacterMovement()->MaxWalkSpeed = ChangeSpeed;
				OverlapCharacter->OnAccelEffect();
				GetWorldTimerManager().SetTimer(SpeedRingTImer, [this, OverlapCharacter]()
					{
						OverlapCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
						OverlapCharacter->OffAccelEffect();
					}
				, DurationTime, false);

			}
			else
			{
				//SendPacket
				//send_obstacle_packet(Network::GetNetwork()->s_socket,ObjectID??);
			}
		}
	}
}


