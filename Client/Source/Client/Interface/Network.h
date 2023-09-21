// Fill out your copyright notice in the Description page of Project Settings.
// 언리얼 클래스에서 헤더파일은 generated.h아래에 추가하면 에러가 발생하므로 위에 추가해야합니다.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Socket.h"
#include "Network.generated.h"

UINTERFACE(MinimalAPI)
class UNetwork : public UInterface
{
	GENERATED_BODY()
};


class CLIENT_API INetwork
{
	GENERATED_BODY()

public:

	//Server code
	
	

	//virtual bool ConnectServer() = 0;
	//virtual void RecvPacket() = 0;
};
