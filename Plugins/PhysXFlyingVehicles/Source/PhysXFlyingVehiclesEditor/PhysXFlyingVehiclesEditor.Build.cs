// 

using UnrealBuildTool;

public class PhysXFlyingVehiclesEditor : ModuleRules
{
    public PhysXFlyingVehiclesEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                 "Core",
                 "CoreUObject",
                 "Slate",
                 "SlateCore",
                 "Engine",
                 "UnrealEd",
                 "PropertyEditor",
                 "AnimGraphRuntime",
                 "AnimGraph",
                 "BlueprintGraph",
                 "PhysXFlyingVehicles",
                 "PhysicsCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                 "EditorStyle",
                 "AssetRegistry"
            }
        );
    }
}
