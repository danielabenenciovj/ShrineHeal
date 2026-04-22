// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShrineHeal : ModuleRules
{
	public ShrineHeal(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
            "Niagara"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ShrineHeal",
			"ShrineHeal/Variant_Platforming",
			"ShrineHeal/Variant_Platforming/Animation",
			"ShrineHeal/Variant_Combat",
			"ShrineHeal/Variant_Combat/AI",
			"ShrineHeal/Variant_Combat/Animation",
			"ShrineHeal/Variant_Combat/Gameplay",
			"ShrineHeal/Variant_Combat/Interfaces",
			"ShrineHeal/Variant_Combat/UI",
			"ShrineHeal/Variant_SideScrolling",
			"ShrineHeal/Variant_SideScrolling/AI",
			"ShrineHeal/Variant_SideScrolling/Gameplay",
			"ShrineHeal/Variant_SideScrolling/Interfaces",
			"ShrineHeal/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
