// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "Door.generated.h"

enum EOpenType
{
	WaitToOpen,
	OpenToWait,
	WaitToClose,
	CloseToWait
};

UCLASS()
class CLIENT_API ADoor : public ABaseObstacle
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
	//���� ���� ���� ������ �Լ�

	void SetStartOpenDoor();
	void SetStartCloseDoor();

	// �ӽ������� �ð��� ���ϴ� �Լ�.
	// �����δ� �������� Ư���� �ð� (chrono���� Ȱ���� ��Ȯ�� �ð�)�� �޾� �����ؾ� �� ���� ����.
	void SetBaseTime(float value);

	//���� ���� � �������� �˷��ִ� �Լ�
	// Opening�� true�� ������ �ִ� ����,
	// Closing�� true�� ������ �ִ� ����.
	// �Ѵ� false�� ��쿡�� ������ �ִ� ������.
	bool bIsOpeningDoor = false;
	bool bIsClosingDoor = false;

	// ���� ���� �� �󸶳� �̵��ؾ� �ϴ� �� ���� �Ÿ��� ����ϱ� ���� ����
	float fRemainMoveDist = 0.f;

	// �������� �ϴ� �ð�. ����ȭ�� ��� ������ �˷��ְ� �� ���� ����.
	float fBaseTime = 0.f;

	// �������� �ϴ� �ð����� ���� ����ϱ� ���� �ð�.
	float fCalcTime = 0.f;

	// ������ �ؾ��� �۾��� �����ϱ� ���� ����
	EOpenType OpenType = WaitToOpen;

	// ���� �����ų� ������ ����, �󸶳� ��ٷ��� ����.
	// �� ���ڴ� �ʹ� ������ ������ �߻��� �� ����.
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float DoorOpenTerm = 2.5f;

	// ���� ������ ������ �ӵ�
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float DoorMoveSpeed = 200.f;
	
	// ���� ���� ���� �󸶳� ������ �ϴ���
	UPROPERTY(EditAnyWhere, Category = Stat, Meta = (AllowPrivateAccess = true)) float DoorOpenDistance = 100.f;
};
