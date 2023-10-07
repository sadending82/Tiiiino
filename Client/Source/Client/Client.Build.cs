// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Client : ModuleRules
{
	public Client(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay",
			"EnhancedInput", "AnimGraphRuntime", "UMG","GeometryCollectionEngine" });

        //����η� ��ϰ���
        PublicIncludePaths.Add(ModuleDirectory);

        bUseUnity = false;
    }
}
