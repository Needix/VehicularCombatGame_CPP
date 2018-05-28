// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VehicleCombatGameCPP : ModuleRules
{
	public VehicleCombatGameCPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysXVehicles", "HeadMountedDisplay" });

        PublicDependencyModuleNames.AddRange(new string[] {
             "OnlineSubsystem",
             "OnlineSubsystemUtils",
             "Landscape",
             "Steamworks",
             "UMG", "Slate", "SlateCore",
         });

        PublicDependencyModuleNames.AddRange(new string[] {
             "OnlineSubsystem", "OnlineSubsystemUtils"
        });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

        PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");
	}
}
