// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MapMakerEditor : ModuleRules
{
	public MapMakerEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "MapMakerEditor/Private",
                "MapMakerEditor/Public",
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
				"Projects",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "MapMakerRuntime",
                "AssetTools",
                "Slate",
                "InputCore",
                "SlateCore",
                "GraphEditor",
                "PropertyEditor",
                "EditorFramework",
                "EditorStyle",
                "Kismet",
                "KismetWidgets",
                "ApplicationCore",
                "ToolMenus",
				// ... add private dependencies that you statically link with here ...	
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
