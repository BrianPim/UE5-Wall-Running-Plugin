// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WallRunning : ModuleRules
{
	public WallRunning(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
