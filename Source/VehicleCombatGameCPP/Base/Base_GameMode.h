// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "AI/AI_Controller.h"
#include "GameModes/Team.h"
#include "Player/Player_Controller.h"
#include "UI/Widget_Gameplay.h"

#include "Base_GameMode.generated.h"

/**
 *
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API ABase_GameMode : public AGameMode {
	GENERATED_BODY()

	// Variables
  private:
	UWidget_Gameplay *Widget_Gameplay;
	// Level Boundary
	FVector LevelBoundary1 = FVector(0, 0, 0);
	FVector LevelBoundary2 = FVector(0, 0, 0);
	bool LevelBoundarySet = false;

	// Color
	TArray<FVector> TeamColors; 

  public:
	const static int CONST_DeathRespawnTime = 3;
	const static int MaxTeams = 4;
	const static int MaxCarsPerTeam = 2;
	TArray<ATeam *> Teams;

	// Functions
  private:
	void FindLevelBoundaries();
	void CreateTeams();
	void AddPlayerToAnyTeam(APlayer_Controller *pc);
	ATeam* FindTeamForPlayer(APlayer_Controller* pc);

  public:
	ABase_GameMode();
	void Tick(float Delta) override;
	void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	FVector GetRandomTerrainLocation();
	float GetMaxX();
	float GetMaxY();
	float GetMinX();
	float GetMinY();

	float GetRespawnTime();

	virtual TSubclassOf<ABase_DrivePawn> GetAIPawnClass();
	virtual TSubclassOf<AAIController> GetAIControllerClass();
};
