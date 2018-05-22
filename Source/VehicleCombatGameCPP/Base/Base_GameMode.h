// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameModes/Team.h"
#include "Base_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API ABase_GameMode : public AGameMode
{
	GENERATED_BODY()

	// Variables
	private:
		FVector LevelBoundary1 = FVector(0, 0, 0);
		FVector LevelBoundary2 = FVector(0, 0, 0);
		bool LevelBoundarySet = false;

	public:
		const int CONST_DeathRespawnTime = 3;
		int MaxTeams = 4;
		int MaxCarsPerTeam = 2;
		TArray<ATeam*> Teams;
	
	// Functions
	private:
		void FindLevelBoundaries();
		void CreateTeams();
		void SpawnPlayer(ATeam* team);
		void SpawnAIs();
	
	public:
		ABase_GameMode();
		void Tick(float Delta) override;
		void BeginPlay() override;

		float GetRespawnTime();
};
