// Fill out your copyright notice in the Description page of Project Settings.

#include "Team.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "Landscape.h"
#include "Helper/GeneralHelper.h"


// Sets default values
ATeam::ATeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InitCustomComponents();
}

void ATeam::InitCustomComponents() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
}

// Called when the game starts or when spawned
void ATeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ABase_DrivePawn* ATeam::SpawnCar(AController* controller, UClass* driveClass) {
	int tries = 0;
	while(true) {
		FVector currentLocation = GetActorLocation();
		float randomX = UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
		float randomY = UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
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
