// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class TreasuredCoveTarget : TargetRules
{
	public TreasuredCoveTarget(TargetInfo Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        Type = TargetType.Game;
        ExtraModuleNames.AddRange( new string[] { "TreasuredCove" } );

        if (bBuildEditor)
        {
            ExtraModuleNames.AddRange(new string[]
            {
                "TreasuredCoveEditor"
            });
        }
    }
}
