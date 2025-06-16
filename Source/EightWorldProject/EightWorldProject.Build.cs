// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EightWorldProject : ModuleRules
{
	public EightWorldProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "UMG", "NavigationSystem", "OnlineSubsystem", "OnlineSubsystemSteam", "Niagara"  });
	}
}
