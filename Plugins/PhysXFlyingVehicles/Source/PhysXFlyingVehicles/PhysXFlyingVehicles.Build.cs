// 

using UnrealBuildTool;

public class PhysXFlyingVehicles : ModuleRules
{
    public PhysXFlyingVehicles(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "EngineSettings",
                    "RenderCore",
                    "AnimGraphRuntime",
                    "RHI"
            }
            );

        SetupModulePhysicsSupport(Target);
    }
}

