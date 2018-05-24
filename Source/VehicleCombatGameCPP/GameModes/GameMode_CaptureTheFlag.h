// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Base_GameMode.h"
#include "GameModes/CTF/CTF_Flag.h"
#include "GameMode_CaptureTheFlag.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AGameMode_CaptureTheFlag : public ABase_GameMode
{
	GENERATED_BODY()
	
	private:
		UPROPERTY()
		ACTF_Flag* TheFlag = nullptr;
	
	public:
		virtual void Tick(float Delta) override;

		void SpawnFlag();

		ACTF_Flag* getFlag() { return TheFlag; }
};
