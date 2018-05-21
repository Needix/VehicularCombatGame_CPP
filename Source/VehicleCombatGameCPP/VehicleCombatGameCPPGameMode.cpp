// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleCombatGameCPPGameMode.h"
#include "VehicleCombatGameCPPPawn.h"
#include "VehicleCombatGameCPPHud.h"

AVehicleCombatGameCPPGameMode::AVehicleCombatGameCPPGameMode()
{
	DefaultPawnClass = AVehicleCombatGameCPPPawn::StaticClass();
	HUDClass = AVehicleCombatGameCPPHud::StaticClass();
}
