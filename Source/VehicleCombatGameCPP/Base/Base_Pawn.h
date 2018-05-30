// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class ABase_DrivePawn;
class ABase_GunPawn;

/**
 * 
 */
class VEHICLECOMBATGAMECPP_API Base_Pawn {
	public:
		Base_Pawn();
		~Base_Pawn();

	public:
		static void SwitchRoles(ABase_DrivePawn* drivePawn, ABase_GunPawn* gunPawn);
};
