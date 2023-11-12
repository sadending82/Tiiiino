// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Client : ModuleRules
{
	public Client(ReadOnlyTargetRules Target) : base(Target)
	{
		//언리얼에서 try-catch를 사용하려면 아래 코드를 추가해야합니다.
		bEnableExceptions = true;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay",
			"EnhancedInput", "AnimGraphRuntime", "UMG","GeometryCollectionEngine"
		,"FieldSystemEngine" , "PhysicsCore"});
        //상대경로로 등록가능
        PublicIncludePaths.Add(ModuleDirectory);

        bUseUnity = false;
    }
}
