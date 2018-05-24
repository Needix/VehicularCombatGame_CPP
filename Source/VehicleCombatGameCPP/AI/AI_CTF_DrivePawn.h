// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/CTF/CTF_Flag.h"
#include "AI/AI_DrivePawn.h"
#include "AI_CTF_DrivePawn.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AAI_CTF_DrivePawn : public AAI_DrivePawn
{
	GENERATED_BODY()
	
	public:
		virtual void Tick(float delta) override;

	protected:
		ACTF_Flag* FindFlag();	
		void SetSteering(AActor* actor) override;
		void SetThrottle(AActor* actor) override;
};
