// Fill out your copyright notice in the Description page of Project Settings.

// Engine Header
#include "Base_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Landscape.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
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
		int tries = 0;
		while (true) {
			float x1 = LevelBoundary1.X;
			float x2 = LevelBoundary2.X;
			float y1 = LevelBoundary1.Y;
			float y2 = LevelBoundary2.Y;
			float locX = UKismetMathLibrary::RandomFloatInRange(UKismetMathLibrary::Min(x1, x2), UKismetMathLibrary::Max(x1, x2));
			float locY = UKismetMathLibrary::RandomFloatInRange(UKismetMathLibrary::Min(y1, y2), UKismetMathLibrary::Max(y1, y2));

			FVector lineTraceStart = FVector(locX, locY, GeneralHelper::LandscapeZStart);
			FVector lineTraceEnd = FVector(locX, locY, GeneralHelper::LandscapeZEnd);

			FHitResult outHit;
			TArray<AActor *> actorsToIgnore;
			TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
			objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
			bool traceSuccess = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), lineTraceStart, lineTraceEnd, objectTypes, false, actorsToIgnore, EDrawDebugTrace::None, outHit, true);

			if (traceSuccess && outHit.Actor->GetClass()->IsChildOf(ALandscape::StaticClass())) {
				outHit.Location.Z = outHit.Location.Z + 100;
				FTransform spawnTransform = FTransform(outHit.Location);
				FActorSpawnParameters spawnParameters;
				spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ATeam *team = GetWorld()->SpawnActor<ATeam>(ATeam::StaticClass(), spawnTransform, spawnParameters);
				team->Setup(FString(TEXT("Team ") + (i + 1)), i, TeamColors[i]);
				Teams.Add(team);
				break;
			}
			tries++;
			if (tries == 20) {
				UE_LOG(LogTemp, Warning, TEXT("Base_GameMode: Currently at %i tries to create teams! Trying 10 more tries..."), tries);
			} else if (tries == 30) {
				UE_LOG(LogTemp, Fatal, TEXT("Base_GameMode: Could not spawn teams after %i tries! Aborting..."), tries);
			}
		}
	}
}

void ABase_GameMode::SpawnPlayer(ATeam *team) {
	APlayerController *pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABase_DrivePawn *spawnedCar = team->SpawnCar(pc, APlayer_DrivePawn::StaticClass());

	APlayer_Controller *playerController = CastChecked<APlayer_Controller>(pc);
	playerController->Team = team;
}

void ABase_GameMode::SpawnAIs() {
	for (ATeam *team : Teams) {
		TArray<AController *> controllerArray = team->TeamPlayer;
		for (int i = controllerArray.Num(); i < MaxCarsPerTeam; i++) {
			ABase_DrivePawn *spawnedCar = team->SpawnCar(NULL, AAI_DrivePawn::StaticClass());
		}
	}
}

float ABase_GameMode::GetRespawnTime() {
	return CONST_DeathRespawnTime;
}
