// Fill out your copyright notice in the Description page of Project Settings.

#include "Team.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Landscape.h"

#include "AI/AI_DrivePawn.h"
#include "Helper/GeneralHelper.h"
#include "Base/Base_GameMode.h"


// Sets default values
ATeam::ATeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
}

// Called when the game starts or when spawned
void ATeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATeam::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	HandleAIRespawn(DeltaTime);
}

void ATeam::HandleAIRespawn(float DeltaTime) {
	AI_RespawnTimer += DeltaTime;

	ABase_GameMode* gameMode = CastChecked<ABase_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(AI_RespawnTimer < gameMode->GetRespawnTime()) {
		return;
	}

	if(TeamPlayer.Num() < gameMode->MaxCarsPerTeam) {
		SpawnCar(NULL, AAI_DrivePawn::StaticClass());
		AI_RespawnTimer = 0;
		return;
	}

	for(int i = 0; i < TeamPlayer.Num(); i++) {
		AController* controller = TeamPlayer[i];

		if(!IsValid(controller)) {
			SpawnCar(NULL, AAI_DrivePawn::StaticClass(), i);
			AI_RespawnTimer = 0;
			return;
		}
	}
}

ABase_DrivePawn* ATeam::SpawnCar(AController* controller, UClass* driveClass) {
	return SpawnCar(controller, driveClass, -1);
}
ABase_DrivePawn* ATeam::SpawnCar(AController* controller, UClass* driveClass, int controllerIndex) {
	int tries = 0;
	UE_LOG(LogTemp, Display, TEXT("Trying to spawn car for %s for team %s"), controller == NULL ? TEXT("") : *controller->GetName(), *this->GetName());
	while(true) {
		FVector currentLocation = GetActorLocation();
		float randomX = this->GetActorLocation().X + UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
		float randomY = this->GetActorLocation().Y + UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
		FVector lineTraceStart = FVector(randomX, randomY, GeneralHelper::LandscapeZStart);
		FVector lineTraceEnd = FVector(randomX, randomY, GeneralHelper::LandscapeZEnd);

		// Make line trace if we hit a landscape (prevent spawn over water for example)
		FHitResult outHit;
		TArray<AActor*> actorsToIgnore;
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		bool traceSuccess = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), lineTraceStart, lineTraceEnd, objectTypes, false, actorsToIgnore, EDrawDebugTrace::None, outHit, true);

		// Successfull?
		if(traceSuccess && outHit.Actor->GetClass()->IsChildOf(ALandscape::StaticClass())) { // TODO: This and lineTrace is the same as in Base_GameMode.css in function CreateTeams. Mabye combine into one function/helper?
			FActorSpawnParameters spawnParameters;
			spawnParameters.Owner = controller;
			ABase_DrivePawn* result = GetWorld()->SpawnActor<ABase_DrivePawn>(driveClass, FTransform(FVector(randomX, randomY, currentLocation.Z)), spawnParameters);
			if(IsValid(result)) {
				if(controller == NULL) {
					result->SpawnDefaultController();
					controller = result->GetController();
				} else {
					controller->Possess(result);
				}

				if(controllerIndex < 0) {
					TeamPlayer.Add(controller);
				} else {
					TeamPlayer[controllerIndex] = controller;
				}
				UE_LOG(LogTemp, Display, TEXT("Car spawned successfull for %s at location %f/%f/%f for team %s"), *controller->GetName(), randomX, randomY, currentLocation.Z, *this->GetName());
				return result;
			}
		}
		tries++;
		if(tries == 20) {
			UE_LOG(LogTemp, Warning, TEXT("Base_GameMode: Currently at %i tries to spawn car! Trying 10 more tries..."), tries);
		} else if(tries == 30) {
			UE_LOG(LogTemp, Fatal, TEXT("Base_GameMode: Could not spawn car after %i tries! Aborting..."), tries);
		}
	}
}
