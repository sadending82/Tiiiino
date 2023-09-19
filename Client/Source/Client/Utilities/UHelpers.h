#pragma once
#include "CoreMinimal.h"
#include "UObject/ConstructorHelpers.h"

class CLIENT_API UHelpers
{
public:

	//에셋 파일을 C++에서 등록할때 사용
	template<typename T>
	static void GetAsset(T** OutObject, FString InPath)
	{
		ConstructorHelpers::FObjectFinder<T> asset(*InPath);
		verifyf(asset.Succeeded(), L"asset.Succeeded()");
		*OutObject = asset.Object;
	}

	//위와 기능은 동일, 게임 중에 에셋 변경시에 사용
	template<typename T>
	static void GetAssetDynamic(T** OutObject, FString InPath)
	{
		T* obj = Cast<T>(StaticLoadObject(T::StaticClass(), NULL, *InPath));
		verifyf(!!obj, L"!!obj");
		*OutObject = obj;
	}

	template<typename T>
	static void CreateComponent(AActor* InActor, T** InComponent,
		FName InName, USceneComponent* InParent = NULL)
	{
		*InComponent = InActor->CreateDefaultSubobject<T>(InName);

		if (!!InParent)
		{
			(*InComponent)->SetupAttachment(InParent);
			return;
		}

		InActor->SetRootComponent((*InComponent));
	}

	template<typename T>
	static void CreateActorComponent(AActor* InActor, T** InComponent, FName InName)
	{
		*InComponent = InActor->CreateDefaultSubobject<T>(InName);
	}

	template<typename T>
	static void GetClass(TSubclassOf<T>* OutClass, FString InPath)
	{
		ConstructorHelpers::FClassFinder<T> asset(*InPath);

		verifyf(asset.Succeeded(), L"asset.Succeeded()");

		*OutClass = asset.Class;
	}

	template<typename T>
	static void GetClassDynamic(TSubclassOf<T>* OutClass, FString InPath)
	{
		TSubclassOf<T> cclass = StaticLoadClass(T::StaticClass(), NULL, *InPath);

		verifyf(!!cclass, L"!! class");

		*OutClass = cclass;
	}

	template<typename T>
	static void FindActors(class UWorld* InWorld, TArray<T*>& OutActors)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(InWorld, T::StaticClass(), actors);

		for (AActor* actor : actors)
			OutActors.Add(Cast<T>(actor));
	}
};