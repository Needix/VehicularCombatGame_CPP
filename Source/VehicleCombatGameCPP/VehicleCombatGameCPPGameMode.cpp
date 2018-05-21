// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleCombatGameCPPGameMode.h"
#include "Base/Base_DrivePawn.h"
#include "VehicleCombatGameCPPHud.h"

AVehicleCombatGameCPPGameMode::AVehicleCombatGameCPPGameMode() {
  DefaultPawnClass = ABase_DrivePawn::StaticClass();
  HUDClass = AVehicleCombatGameCPPHud::StaticClass();
}
