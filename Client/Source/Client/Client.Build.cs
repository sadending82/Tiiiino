// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Client : ModuleRules
{
	public Client(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay",
<<<<<<< Updated upstream
			"EnhancedInput", "AnimGraphRuntime" });
=======
			"EnhancedInput", "AnimGraphRuntime", "UMG","GeometryCollectionEngine" });
>>>>>>> Stashed changes

        //����η� ��ϰ���
        PublicIncludePaths.Add(ModuleDirectory);
    }
}
