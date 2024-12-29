// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class TreasuredCoveEditorTarget : TargetRules
{
	public TreasuredCoveEditorTarget(TargetInfo Target) : base(Target)
	{

        DefaultBuildSettings = BuildSettingsVersion.Latest;
        bOverrideBuildEnvironment = true;
        Type = TargetType.Editor;

        ExtraModuleNames.AddRange(new string[] { "TreasuredCoveEditor" });
    }
}
