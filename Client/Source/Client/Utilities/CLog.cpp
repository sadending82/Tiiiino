#include "Utilities/CLog.h"
#include "Engine.h" //많은 것이 포함되어서 무거우므로 필요할때만 사용할 것

DEFINE_LOG_CATEGORY_STATIC(GameProject, Display, All)

void CLog::Print(int32 InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, FString::FromInt(InValue));
}

void CLog::Print(float InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, FString::SanitizeFloat(InValue));
}

void CLog::Print(const FString& InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, InValue);
}

void CLog::Print(const FVector& InValue, int32 InKey, float Duration, FColor InColor)
{
	//UObject이하는 ToString함수를 갖고있음
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, InValue.ToString());

}

void CLog::Print(const FRotator& InValue, int32 InKey, float Duration, FColor InColor)
{
	GEngine->AddOnScreenDebugMessage(InKey, Duration, InColor, InValue.ToString());
}

void CLog::Log(int32 InValue)
{
	//카테고리와 표현형식이 가능한 매크로
	UE_LOG(GameProject, Display, L"%d", InValue);
}

void CLog::Log(float InValue)
{
	UE_LOG(GameProject, Display, L"%f", InValue);
}

void CLog::Log(const FString& InValue)
{
	//Unreal log에 들어올때 String은 포인터가 되어야함
	UE_LOG(GameProject, Display, L"%s", *InValue);
}

void CLog::Log(const FVector& InValue)
{
	UE_LOG(GameProject, Display, L"%s", *InValue.ToString());
}

void CLog::Log(const FRotator& InValue)
{
	UE_LOG(GameProject, Display, L"%s", *InValue.ToString());
}

void CLog::Log(const UObject* InObject)
{
	FString str;
	if (!!InObject)
		str.Append(InObject->GetName());
	str.Append(!!InObject ? "Not Null" : "Null");
	UE_LOG(GameProject, Display, L"%s", *str);
}

void CLog::Log(const FString& InFuncName, int32 InlineNumber)
{
	FString str;
	str.Append(InFuncName);
	str.Append(",");
	str.Append(FString::FromInt(InlineNumber));

	UE_LOG(GameProject, Display, L"%s", *str);
}
