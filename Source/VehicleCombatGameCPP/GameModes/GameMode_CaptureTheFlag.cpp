// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_CaptureTheFlag.h"

#include "Engine/Engine.h"

#include "GameModes/CTF/CTF_Flag.h"

void AGameMode_CaptureTheFlag::Tick(float delta) {
	Super::Tick(delta);
	if(TheFlag == nullptr) {
		SpawnFlag();
	}
}


void AGameMode_CaptureTheFlag::SpawnFlag() {
	FTransform location = FTransform(GetRandomTerrainLocation());
	FActorSpawnParameters spawnParameters = FActorSpawnParameters();
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TheFlag = GetWorld()->SpawnActor<ACTF_Flag>(ACTF_Flag::StaticClass(), location, spawnParameters);
}
