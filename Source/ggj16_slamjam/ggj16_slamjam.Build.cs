// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ggj16_slamjam : ModuleRules
{
	public ggj16_slamjam(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Paper2D" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });
	}
}
