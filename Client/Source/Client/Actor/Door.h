// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

enum EOpenType
{
	WaitToOpen,
	OpenToWait,
	WaitToClose,
	CloseToWait
};

UCLASS()
class CLIENT_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* LeftMesh;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* RightMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//문의 열고 닫힘 조정용 함수

	void SetStartOpenDoor();
	void SetStartCloseDoor();

	// 임시적으로 시간을 정하는 함수.
	// 실제로는 서버에서 특정한 시간 (chrono등을 활용한 정확한 시간)을 받아 실행해야 할 수도 있음.
	void SetBaseTime(float value);

	//현재 문이 어떤 상태인지 알려주는 함수
	// Opening이 true면 열리고 있는 상태,
	// Closing이 true면 닫히고 있는 상태.
	// 둘다 false인 경우에는 가만히 있는 상태임.
	bool bIsOpeningDoor = false;
	bool bIsClosingDoor = false;

	// 열고 닫힐 때 얼마나 이동해야 하는 지 남은 거리를 계산하기 위한 변수
	float fRemainMoveDist = 0.f;

	// 기준으로 하는 시간. 동기화의 경우 서버가 알려주게 될 수도 잇음.
	float fBaseTime = 0.f;

	// 기준으로 하는 시간으로 부터 계산하기 위한 시간.
	float fCalcTime = 0.f;

	// 다음에 해야할 작업을 저장하기 위한 변수
	EOpenType OpenType = WaitToOpen;

	// 문이 열리거나 닫히고 나서, 얼마나 기다려야 할지.
	// 이 숫자는 너무 작으면 에러가 발생할 수 있음.
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float DoorOpenTerm = 2.5f;

	// 문이 열리고 닫히는 속도
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float DoorMoveSpeed = 200.f;
	
	// 문이 열릴 때에 얼마나 열려야 하는지
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float DoorOpenDistance = 100.f;
};
