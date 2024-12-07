// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JsonAssetEditor : ModuleRules
{
	public JsonAssetEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "JsonAssetEditor/Private",
                "JsonAssetEditor/Public",
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
                "Engine",
                "UnrealEd",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "JsonAssetRuntime",
                "CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
                "AssetTools",
                "ApplicationCore",
                "InputCore",
                "ToolMenus",
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
