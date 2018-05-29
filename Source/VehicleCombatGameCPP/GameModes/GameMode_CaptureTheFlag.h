// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Base/Base_GameMode.h"
#include "GameModes/CTF/CTF_Flag.h"
#include "AI/AI_CTF_Controller.h"

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
		AGameMode_CaptureTheFlag();

		virtual void Tick(float Delta) override;

		void SpawnFlag();

		ACTF_Flag* getFlag() { return TheFlag; }
		

		TSubclassOf<AAIController> GetAIControllerClass() override {
			return AAI_CTF_Controller::StaticClass();
		}
};
