// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TPS_005_git : ModuleRules
{
	public TPS_005_git(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"SlateCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			"TPS_005_git",
			"TPS_005_git/Variant_Platforming",
			"TPS_005_git/Variant_Platforming/Animation",
			"TPS_005_git/Variant_Combat",
			"TPS_005_git/Variant_Combat/AI",
			"TPS_005_git/Variant_Combat/Animation",
			"TPS_005_git/Variant_Combat/Gameplay",
			"TPS_005_git/Variant_Combat/Interfaces",
			"TPS_005_git/Variant_Combat/UI",
			"TPS_005_git/Variant_SideScrolling",
			"TPS_005_git/Variant_SideScrolling/AI",
			"TPS_005_git/Variant_SideScrolling/Gameplay",
			"TPS_005_git/Variant_SideScrolling/Interfaces",
			"TPS_005_git/Variant_SideScrolling/UI",
			"TPS_005_git/ThirdPerson"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
