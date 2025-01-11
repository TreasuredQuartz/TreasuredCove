// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CharacterClassMakerEditor : ModuleRules
{
	public CharacterClassMakerEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
                "CharacterClassMakerEditor/Private",
                "CharacterClassMakerEditor/Public",
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CharacterClassMakerRuntime",
				// ... add private dependencies that you statically link with here ...	
				"Slate",
                "SlateCore",
                "AssetTools",
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
