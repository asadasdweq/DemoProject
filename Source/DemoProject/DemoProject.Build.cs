// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DemoProject : ModuleRules
{
	public DemoProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", 
	        "EnhancedInput",
	        "GameplayAbilities",
	        "GameplayTags",
	        "GameplayTasks",
	        "UMG",
	        "Slate",
	        "SlateCore",
	        "AIModule",
	        "NetCore",
	        "StructUtils"
        });
    }
}
