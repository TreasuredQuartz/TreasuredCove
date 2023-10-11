// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class TreasuredCoveEditor : ModuleRules
{
    public TreasuredCoveEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "InputCore",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GenericGraphRuntime",
            "GameplayAbilities",
            "TreasuredCove",
        });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] 
        { 
            "PropertyEditor",
            "Slate",
            "SlateCore",
            "UnrealEd",
        });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.AddRange(new string[] {  });

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}