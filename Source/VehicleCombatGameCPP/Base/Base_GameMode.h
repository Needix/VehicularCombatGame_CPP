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

	private:
		FVector LevelBoundary1 = FVector(0, 0, 0);
		FVector LevelBoundary2 = FVector(0, 0, 0);

	public:
		const int CONST_DeathRespawnTime = 3;
		int MaxTeams = 4;
		int MaxCarsPerTeam = 2;
		TArray<ATeam> teams;
	
	private:
		void FindLevelBoundaries();
		void CreateTeams();
		void SpawnPlayer();
		void SpawnAIs();
	
	public:
		void BeginPlay() override;
};
