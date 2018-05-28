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

	static ConstructorHelpers::FObjectFinder<USoundCue> backgroundMusicObject(TEXT("/Game/VehicularCombatGame/Sound/BackgroundMusic_Cue.BackgroundMusic_Cue"));
	UGameplayStatics::PlaySound2D(GetWorld(), backgroundMusicObject.Object);
}

void ABase_GameMode::BeginPlay() {

}

void ABase_GameMode::Tick(float Delta) {
	if (!LevelBoundarySet) {
		FindLevelBoundaries();
		CreateTeams();
	} else {
		TArray<AActor *> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayer_Controller::StaticClass(), actors);
		for(AActor* actor : actors) {
			APlayer_Controller* pc = CastChecked<APlayer_Controller>(actor);
			if(!IsValid(pc->Team)) {
				AddPlayerToAnyTeam(pc);
			}
		}
	}
}

void ABase_GameMode::PostLogin(APlayerController* NewPlayer) {
	UE_LOG(LogTemp, Display, TEXT("New player joined (%s)!"), *NewPlayer->GetClass()->GetName());
}

// Initial spawning of stuff
void ABase_GameMode::FindLevelBoundaries() {
	TArray<AActor *> levelBoundaries;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelBoundary::StaticClass(), levelBoundaries);
	check(levelBoundaries.Num() == 2);
	LevelBoundary1 = levelBoundaries[0]->GetActorLocation();
	LevelBoundary2 = levelBoundaries[1]->GetActorLocation();
	LevelBoundarySet = true;
}
void ABase_GameMode::CreateTeams() {
	UE_LOG(LogTemp, Display, TEXT("Creating teams..."));
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
void ABase_GameMode::AddPlayerToAnyTeam(APlayer_Controller *pc) {
	UE_LOG(LogTemp, Display, TEXT("Trying to add player to team: %s"), *pc->GetName());
	ATeam* team = FindTeamForPlayer(pc);
	team->AddPlayer(pc);
}
ATeam* ABase_GameMode::FindTeamForPlayer(APlayer_Controller* pc) {
	ATeam* result = NULL;
	int highestNumberAIInTeam = 0;
	check(Teams.Num() != 0);
	for(ATeam* team : Teams) {
		int currentNumberAIInTeam = team->GetNumberAIInTeam();
		int numberOfControllerInTeam = team->TeamPlayer.Num();
		if((result == NULL && (currentNumberAIInTeam > 0 || numberOfControllerInTeam != MaxCarsPerTeam)) || (highestNumberAIInTeam < currentNumberAIInTeam)) { // Lowest number not set yet OR current team has more space -> set new team as "best"
			result = team;
			highestNumberAIInTeam = currentNumberAIInTeam;
		}
	}
	return result;
}

// Helper for spawning
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
			UE_LOG(LogTemp, Warning, TEXT("Base_GameMode: Currently at %i tries to create teams! Trying 30 more tries..."), i);
		} else if (i == 100) {
			UE_LOG(LogTemp, Fatal, TEXT("Base_GameMode: Could not spawn teams after %i tries! Aborting..."), i);
		}
	}
}
float ABase_GameMode::GetMaxX() { return UKismetMathLibrary::Max( LevelBoundary1.X, LevelBoundary2.X); }
float ABase_GameMode::GetMinX() { return UKismetMathLibrary::Min( LevelBoundary1.X, LevelBoundary2.X); }
float ABase_GameMode::GetMaxY() { return UKismetMathLibrary::Max( LevelBoundary1.Y, LevelBoundary2.Y); }
float ABase_GameMode::GetMinY() { return UKismetMathLibrary::Min( LevelBoundary1.Y, LevelBoundary2.Y); }

// Getter
float ABase_GameMode::GetRespawnTime() {
	return CONST_DeathRespawnTime;
}
UClass* ABase_GameMode::GetAIPawnClass() {
	return AAI_DrivePawn::StaticClass();
}
