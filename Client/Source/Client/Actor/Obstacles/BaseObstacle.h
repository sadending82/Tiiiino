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

	const int GetObjectType() const { return ObjectType; }
	void SetObjectType(const int Type) { ObjectType = Type; }

	const int GetObjectID() const { return mObjectID; }
	void SetObjectID(const int id) { mObjectID = id; }

protected:

	UPROPERTY(EditAnywhere, Category = "Network")
		int ObjectType;

	//1006 �̼��� �� mObjectID�� �μ����� ���ǹ� �μ����� ���� ��ȹ�� �ѹ����� ���־����. "������"
	UPROPERTY(EditAnywhere, Category = "Network")
	int mObjectID;

		// 10/06 ������
		// �� �κ��� ��Ʈ��ũ ����ȭ�� ���� �����Դϴ�.
		// Ÿ�̹��� ����� �ϴ� ��ֹ����� �� ������ true�� �Ǵ� ���� �����̵��� �����ؾ� �մϴ�. 
	UPROPERTY(EditAnywhere, Category = "Network")
		bool bIsStartMove = false;
};
