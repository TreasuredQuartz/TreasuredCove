// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class TreasuredCove : ModuleRules
{
	public TreasuredCove(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"AdvancedSessions", 
			"AdvancedSteamSessions",
			"AIModule",
			"Core", 
			"CoreUObject",
            "ChaosVehicles",
            "Engine", 
			"InputCore", 
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Networking",
			"OnlineSubsystem", 
			"OnlineSubsystemUtils",
			"RHI", 
			"RenderCore",
			"SimplexNoise",
			"Slate",
			"SlateCore",
			"Sockets",
			"UMG" 
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"ProceduralMeshComponent", 
			"RuntimeMeshComponent",
			"GameplayTowns", 
			"PhysXFlyingVehicles", 
			"DlgSystem", 
			"UnrealEd", 
			"PlayFab",
			"SenseSystem"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.AddRange(new string[] {  });

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
