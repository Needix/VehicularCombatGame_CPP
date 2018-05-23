// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameModes/Team.h"
#include "Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API APlayer_Controller : public APlayerController {
	GENERATED_BODY()
	

	public:
		ATeam* Team;
		float RespawnTimer;

	public:
		virtual void Tick(float Delta) override;
	
	
};
