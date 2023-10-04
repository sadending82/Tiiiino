#include "Actor/Obstacles/SpeedRing.h"
//#include "Network/Network.h"
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
	auto OverlapCharacter = Cast<ACharacter>(OtherComp->GetOwner());
	if (OverlapCharacter)
	{
		auto Controller = OverlapCharacter->GetController();
		if (!!Controller)
		{
			if (Controller->IsPlayerController())
			{
				//본인 속도만 바꾼다. 다른 플레이어는 서버에서 수정된 속도로 move packet을 보내줄 듯?
				//초기속도저장은 덮어쓰지않도록
				if(GetWorldTimerManager().IsTimerActive(SpeedRingTImer) == false) 
					OriginalSpeed = OverlapCharacter->GetCharacterMovement()->MaxWalkSpeed;
				else
				{	//타이머 중복 방지, 이미있으면 삭제하고 다시 생성
					GetWorldTimerManager().ClearTimer(SpeedRingTImer);
				}
				OverlapCharacter->GetCharacterMovement()->MaxWalkSpeed = ChangeSpeed;
				GetWorldTimerManager().SetTimer(SpeedRingTImer, [this, OverlapCharacter]()
					{
						OverlapCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
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


