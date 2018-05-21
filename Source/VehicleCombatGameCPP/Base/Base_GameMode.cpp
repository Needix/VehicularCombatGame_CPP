// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AllLevel/LevelBoundary.h"

ABase_GameMode::ABase_GameMode() {
	
}

void ABase_GameMode::BeginPlay() {
  FindLevelBoundaries();
  CreateTeams();
  SpawnPlayer();
  SpawnAIs();
}

void ABase_GameMode::FindLevelBoundaries() { 
	TArray<AActor*> levelBoundaries;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelBoundary::StaticClass(), levelBoundaries);
	for(int i = 0; i < levelBoundaries.Num(); i++) {
		ALevelBoundary* levelBoundary = (ALevelBoundary*)levelBoundaries[i];
	}
}

void ABase_GameMode::CreateTeams() {}

void ABase_GameMode::SpawnPlayer() {}

void ABase_GameMode::SpawnAIs() {}
