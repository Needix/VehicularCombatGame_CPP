// Fill out your copyright notice in the Description page of Project Settings.

// Engine Header
#include "Base_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Landscape.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundCue.h"

#include "Components/AudioComponent.h"

#include "GameModes/Team.h"
#include "AI/AI_DrivePawn.h"
#include "AllLevel/LevelBoundary.h"
#include "Helper/GeneralHelper.h"
#include "Player/Player_DrivePawn.h"
#include "Player/Player_Controller.h"

ABase_GameMode::ABase_GameMode() {
	DefaultPawnClass = NULL;
	// HUDClass = AVehicleCombatGameCPPHud::StaticClass();
	PlayerControllerClass = APlayer_Controller::StaticClass();

	TeamColors.Add(FVector(255,	0,		0));
	TeamColors.Add(FVector(0,	255,	0));
	TeamColors.Add(FVector(0,	0,		255));
	TeamColors.Add(FVector(255,	255,	0));
	TeamColors.Add(FVector(255,	0,		255));
	TeamColors.Add(FVector(0,	255,	255));
	TeamColors.Add(FVector(0,	0,		0));
	TeamColors.Add(FVector(255,	255,	255));

	/*static ConstructorHelpers::FObjectFinder<USoundCue> backgroundMusicObject(TEXT("/Game/VehicularCombatGame/Sound/BackgroundMusic_Cue.BackgroundMusic_Cue"));
	UGameplayStatics::PlaySound2D(GetWorld(), backgroundMusicObject.Object);*/
}

void ABase_GameMode::BeginPlay() {

}

void ABase_GameMode::Tick(float Delta) {
	if (!LevelBoundarySet) {
		FindLevelBoundaries();
		CreateTeams();
		SpawnPlayer(Teams[UKismetMathLibrary::RandomInteger(Teams.Num())]); // TODO: Dont put player into random team
		SpawnAIs();
	}
}

void ABase_GameMode::FindLevelBoundaries() {
	TArray<AActor *> levelBoundaries;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelBoundary::StaticClass(), levelBoundaries);
	if (levelBoundaries.Num() < 2) {
		UE_LOG(LogTemp, Fatal, TEXT("Base_GameMode: Could not find enough level boundary objects!"));
		return;
	}
	LevelBoundary1 = levelBoundaries[0]->GetActorLocation();
	LevelBoundary2 = levelBoundaries[1]->GetActorLocation();
	LevelBoundarySet = true;
	/*for(int i = 0; i < levelBoundaries.Num(); i++) {
		ALevelBoundary* levelBoundary = (ALevelBoundary*)levelBoundaries[i];
		levelBoundary->GetActorLocation();
	}*/
}

void ABase_GameMode::CreateTeams() {
	for (int i = 0; i < MaxTeams; i++) {
		FVector location = GetRandomTerrainLocation();
		location.Z = location.Z + 100;

		FTransform spawnTransform = FTransform(location);
		FActorSpawnParameters spawnParameters;
		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATeam *team = GetWorld()->SpawnActor<ATeam>(ATeam::StaticClass(), spawnTransform, spawnParameters);
		team->Setup(FString(TEXT("Team ") + FString::FromInt(i + 1)), i, TeamColors[i]);
		Teams.Add(team);
	}
}

void ABase_GameMode::SpawnPlayer(ATeam *team) {
	APlayerController *pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABase_DrivePawn *spawnedCar = team->SpawnCar(pc, APlayer_DrivePawn::StaticClass());
}

void ABase_GameMode::SpawnAIs() {
	for (ATeam *team : Teams) {
		TArray<AController *> controllerArray = team->TeamPlayer;
		for (int i = controllerArray.Num(); i < MaxCarsPerTeam; i++) {
			ABase_DrivePawn *spawnedCar = team->SpawnCar(NULL, AAI_DrivePawn::StaticClass());
		}
	}
}

FVector ABase_GameMode::GetRandomTerrainLocation() {
	for(int i = 0; true; i++) {
		float locX = UKismetMathLibrary::RandomFloatInRange(GetMinX(), GetMaxX());
		float locY = UKismetMathLibrary::RandomFloatInRange(GetMinY(), GetMaxY());
		FVector lineTraceStart = FVector(locX, locY, GeneralHelper::LandscapeZStart);
		FVector lineTraceEnd = FVector(locX, locY, GeneralHelper::LandscapeZEnd);

		FHitResult outHit;
		TArray<AActor *> actorsToIgnore;
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		bool traceSuccess = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), lineTraceStart, lineTraceEnd, objectTypes, false, actorsToIgnore, EDrawDebugTrace::None, outHit, true);

		if (traceSuccess && outHit.Actor->GetClass()->IsChildOf(ALandscape::StaticClass())) {
			return outHit.Location;
		}
		if (i == 70) {
			UE_LOG(LogTemp, Warning, TEXT("Base_GameMode: Currently at %i tries to create teams! Trying 10 more tries..."), i);
		} else if (i == 100) {
			UE_LOG(LogTemp, Fatal, TEXT("Base_GameMode: Could not spawn teams after %i tries! Aborting..."), i);
		}
	}
}
float ABase_GameMode::GetMaxX() { return UKismetMathLibrary::Max( LevelBoundary1.X, LevelBoundary2.X); }
float ABase_GameMode::GetMinX() { return UKismetMathLibrary::Min( LevelBoundary1.X, LevelBoundary2.X); }
float ABase_GameMode::GetMaxY() { return UKismetMathLibrary::Max( LevelBoundary1.Y, LevelBoundary2.Y); }
float ABase_GameMode::GetMinY() { return UKismetMathLibrary::Min( LevelBoundary1.Y, LevelBoundary2.Y); }

float ABase_GameMode::GetRespawnTime() {
	return CONST_DeathRespawnTime;
}
