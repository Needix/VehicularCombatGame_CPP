// Fill out your copyright notice in the Description page of Project Settings.

// Engine Header
#include "Team.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Landscape.h"
#include "UObject/ConstructorHelpers.h"

// Component Header
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"

// Custom Header
#include "AI/AI_DrivePawn.h"
#include "Helper/GeneralHelper.h"
#include "Base/Base_GameMode.h"
#include "Player/Player_Controller.h"
#include "Player/Player_DrivePawn.h"

// Sets default values
ATeam::ATeam() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> baseParticleSystem(TEXT("ParticleSystem'/Game/VehicularCombatGame/ParticleEffects/CTF_HomeParticleSystem.CTF_HomeParticleSystem'"));
	baseParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Base_ParticleSystem"));
	baseParticleSystemComponent->Template = baseParticleSystem.Object;
	baseParticleSystemComponent->SetupAttachment(RootComponent);
	
	UBoxComponent* collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	collisionBox->SetRelativeScale3D(FVector(30, 30, 10));
	collisionBox->SetupAttachment(RootComponent);
}

void ATeam::Setup(FString pName, int pId, FVector pColor) {
	Name = pName;
	Id = pId;
	Color = pColor;

	FParticleSysParam sysParam = FParticleSysParam();
	sysParam.Name = FName(TEXT("PSpawn"));
	sysParam.ParamType = PSPT_Scalar;
	sysParam.Scalar = 0.1;
	baseParticleSystemComponent->InstanceParameters.Add(sysParam);

	FParticleSysParam sysParam2 = FParticleSysParam();
	sysParam2.Name = FName(TEXT("PColor"));
	sysParam2.ParamType = PSPT_Vector;
	sysParam2.Vector = FVector(1, 0, 0);
	baseParticleSystemComponent->InstanceParameters.Add(sysParam2);

	FParticleSysParam sysParam3 = FParticleSysParam();
	sysParam3.Name = FName(TEXT("PCylinder"));
	sysParam3.ParamType = PSPT_Scalar;
	sysParam3.Scalar = 0.4;
	baseParticleSystemComponent->InstanceParameters.Add(sysParam3);

	FParticleSysParam sysParam4 = FParticleSysParam();
	sysParam4.Name = FName(TEXT("None"));
	sysParam4.ParamType = PSPT_Actor;
	sysParam4.Actor = NULL;
	baseParticleSystemComponent->InstanceParameters.Add(sysParam4);

	FParticleSysParam sysParam5 = FParticleSysParam();
	sysParam5.Name = FName(TEXT("SmokeEmitterColor"));
	sysParam5.ParamType = PSPT_Vector;
	sysParam5.Vector = pColor;
	baseParticleSystemComponent->InstanceParameters.Add(sysParam5);

}

// Called when the game starts or when spawned
void ATeam::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ATeam::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	HandlePlayerRespawn(DeltaTime);
	HandleAIRespawn(DeltaTime);
}

void ATeam::HandlePlayerRespawn(float DeltaTime) {
	for(int i = 0; i < TeamPlayer.Num(); i++) {
		if(TeamPlayer[i]->GetClass()->IsChildOf(APlayer_Controller::StaticClass())) {
			APlayer_Controller* aPlayerController = CastChecked<APlayer_Controller>(TeamPlayer[i]);
			if(aPlayerController->RespawnTimer > GeneralHelper::PlayerRespawnTime) {
				ABase_DrivePawn* newCar = SpawnCar(aPlayerController, APlayer_DrivePawn::StaticClass(), i);
			}
		}
	}
}

void ATeam::HandleAIRespawn(float DeltaTime) {
	AI_RespawnTimer += DeltaTime;

	ABase_GameMode *gameMode = CastChecked<ABase_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (AI_RespawnTimer < gameMode->GetRespawnTime()) {
		return;
	}

	if (TeamPlayer.Num() < gameMode->MaxCarsPerTeam) {
		SpawnCar(NULL, AAI_DrivePawn::StaticClass());
		AI_RespawnTimer = 0;
		return;
	}

	for (int i = 0; i < TeamPlayer.Num(); i++) {
		AController *controller = TeamPlayer[i];

		if (!IsValid(controller)) {
			SpawnCar(NULL, AAI_DrivePawn::StaticClass(), i);
			AI_RespawnTimer = 0;
			return;
		}
	}
}

ABase_DrivePawn *ATeam::SpawnCar(AController *controller, UClass *driveClass) {
	return SpawnCar(controller, driveClass, -1);
}
ABase_DrivePawn *ATeam::SpawnCar(AController *controller, UClass *driveClass, int controllerIndex) {
	int tries = 0;
	UE_LOG(LogTemp, Display, TEXT("Trying to spawn car for %s for team %s"), controller == NULL ? TEXT("") : *controller->GetName(), *this->GetName());
	while (true) {
		FVector currentLocation = GetActorLocation();
		float randomX = this->GetActorLocation().X + UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
		float randomY = this->GetActorLocation().Y + UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
		FVector lineTraceStart = FVector(randomX, randomY, GeneralHelper::LandscapeZStart);
		FVector lineTraceEnd = FVector(randomX, randomY, GeneralHelper::LandscapeZEnd);

		// Make line trace if we hit a landscape (prevent spawn over water for example)
		FHitResult outHit;
		TArray<AActor *> actorsToIgnore;
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
		objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		bool traceSuccess = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), lineTraceStart, lineTraceEnd, objectTypes, false, actorsToIgnore, EDrawDebugTrace::None, outHit, true);

		// Successfull?
		if (traceSuccess && outHit.Actor->GetClass()->IsChildOf(ALandscape::StaticClass())) { // TODO: This and lineTrace is the same as in Base_GameMode.css in function CreateTeams. Mabye combine into one function/helper?
			FActorSpawnParameters spawnParameters;
			spawnParameters.Owner = controller;
			ABase_DrivePawn *result = GetWorld()->SpawnActor<ABase_DrivePawn>(driveClass, FTransform(FVector(randomX, randomY, currentLocation.Z)), spawnParameters);
			if (IsValid(result)) {
				if (controller == NULL) {
					result->SpawnDefaultController();
					controller = result->GetController();
				} else {
					controller->Possess(result);
					APlayer_Controller *playerController = CastChecked<APlayer_Controller>(controller);
					playerController->Team = this;
				}
				result->Team = this;

				if (controllerIndex < 0) {
					TeamPlayer.Add(controller);
				} else {
					TeamPlayer[controllerIndex] = controller;
				}
				UE_LOG(LogTemp, Display, TEXT("Car spawned successfull for %s at location %f/%f/%f for team %s"), *controller->GetName(), randomX, randomY, currentLocation.Z, *this->GetName());
				return result;
			}
		}
		tries++;
		if (tries == 20) {
			UE_LOG(LogTemp, Warning, TEXT("Base_GameMode: Currently at %i tries to spawn car! Trying 10 more tries..."), tries);
		} else if (tries == 30) {
			UE_LOG(LogTemp, Fatal, TEXT("Base_GameMode: Could not spawn car after %i tries! Aborting..."), tries);
		}
	}
}
