// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VehicleCombatGameCPPTarget : TargetRules
{
	public VehicleCombatGameCPPTarget(TargetInfo Target) : base(Target)
    {
        bUsesSteam = true;
        Type = TargetType.Game;
		ExtraModuleNames.Add("VehicleCombatGameCPP");
	}
}
