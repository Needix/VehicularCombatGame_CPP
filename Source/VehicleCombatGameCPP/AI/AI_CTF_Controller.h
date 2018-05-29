// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AI/AI_Controller.h"
#include "Base/Base_DrivePawn.h"
#include "Base/Base_GunPawn.h"
#include "GameModes/CTF/CTF_Flag.h"

#include "AI_CTF_Controller.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AAI_CTF_Controller : public AAI_Controller
{
	GENERATED_BODY()
	
	private:
		ACTF_Flag* FindFlag();
		void SetSteering(ABase_DrivePawn* myPawn, ACTF_Flag* flag);
		void SetThrottle(ABase_DrivePawn* myPawn, ACTF_Flag* flag);

	protected:
		virtual void HandleDrivePawn(ABase_DrivePawn* pawn) override;
		virtual void HandleGunPawn(ABase_GunPawn* pawn) override;
	
	
};
