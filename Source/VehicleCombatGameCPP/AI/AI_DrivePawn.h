// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Base_DrivePawn.h"
#include "AI_DrivePawn.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AAI_DrivePawn : public ABase_DrivePawn
{
	GENERATED_BODY()
	
	
	public:
		virtual void Tick(float Delta) override;

	private:
		class APlayer_DrivePawn* FindClosestPlayer();
		void SetSteering(APlayer_DrivePawn* closestPlayer);
		void SetThrottle(APlayer_DrivePawn* closestPlayer);
};
