// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VehicleCombatGameCPPEditorTarget : TargetRules
{
	public VehicleCombatGameCPPEditorTarget(TargetInfo Target) : base(Target)
    {
        bUsesSteam = true;
        Type = TargetType.Editor;
		ExtraModuleNames.Add("VehicleCombatGameCPP");
	}
}
