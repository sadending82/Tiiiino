// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseObstacle.generated.h"

UCLASS()
class CLIENT_API ABaseObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	const int GetObjectID() const { return mObjectID; }
	void SetObjectID(const int id) { mObjectID = id; }
	void SetIsObjectSync(const bool sync) { bIsSyncObject = sync; }

	// 10/08 김혁동
	// 네트워크 동기화를 위한 변수를 제어하기 위한 함수를 추가하였습니다.
	// 아래 함수들을 호출하여 사용하면 될 것으로 보입니다.
	inline const bool IsMoveStart() const { return bIsStartMove; }
	void EnableMoveStart(const bool value) { bIsStartMove = value; }

	//장애물별 기능을 적용할 함수, 상속해서 세부기능 구현
	virtual void ActionObject() {};

protected:

	//1006 이수민 이 mObjectID에 부서지는 발판및 부서지는 문은 기획이 넘버링을 해주어야함. "무조건"
	//1010 이수민 기획이 이거 넘버링 하다가 실수 할수도있으니 코드적으로 자동화 시킴.
	//-> 다시생각해보니 로딩타이밍이 런타임이라 보장되지가 않음,,, 자동화 불가.
	UPROPERTY(EditAnywhere, Category = "Network")
	int mObjectID;
	// 10.09 이수민 동기화가 필요한 obstacle과 아닌 obstacle ex)LinearBelt를 구분짓기 위함.
	//동기화가 필요없다면 false. 기본은 false 기획은 건물설치하고 동기화가 필요하다면
	// (움직이는 타이밍 등) 체크를해주도록한다.
	UPROPERTY(EditAnywhere, Category = "Network")
	bool bIsSyncObject = false ;
		// 10/06 김혁동
		// 이 부분은 네트워크 동기화를 위한 변수입니다.
		// 타이밍을 맞춰야 하는 장애물들은 이 변수가 true가 되는 순간 움직이도록 설정해야 합니다. 
	UPROPERTY(EditAnywhere, Category = "Network")
		bool bIsStartMove = false;
};
