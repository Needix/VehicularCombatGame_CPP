// Fill out your copyright notice in the Description page of Project Settings.

// Engine Header
#include "Team.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Landscape.h"
#include "UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

// Component Header
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"

// Custom Header
#include "AI/AI_DrivePawn.h"
#include "AI/AI_Controller.h"
#include "Helper/GeneralHelper.h"
#include "Base/Base_GameMode.h"
#include "Base/Base_GunPawn.h"
#include "Player/Player_Controller.h"
#include "Player/Player_DrivePawn.h"

#define COLLISION_LANDSCAPE		ECC_GameTraceChannel2

// Sets default values
ATeam::ATeam() {
	//SetReplicates(true);
	bReplicates = true;
	bAlwaysRelevant = true;

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

	baseParticleSystemComponent->InstanceParameters.Empty();
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
void ATeam::OnRep_SetupPropertyName() {
	OnRep_SetupProperty();
}
void ATeam::OnRep_SetupPropertyId() {
	OnRep_SetupProperty();
}
void ATeam::OnRep_SetupPropertyColor() {
	OnRep_SetupProperty();
}
void ATeam::OnRep_SetupProperty() {
	UE_LOG(LogTemp, Warning, TEXT("Calling SetupProperty with: %s %i %f %f %f on %s"), *Name, Id, Color.X, Color.Y, Color.Z, *this->GetName());
	Setup(Name, Id, Color);
}

void ATeam::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
    DOREPLIFETIME(ATeam, Name);
    DOREPLIFETIME(ATeam, Id);
    DOREPLIFETIME(ATeam, Color);
}

// Called when the game starts or when spawned
void ATeam::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ATeam::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if(this->HasAuthority()) {
		HandlePlayerRespawn(DeltaTime);
		HandleAIRespawn(DeltaTime);
	}
}

void ATeam::HandlePlayerRespawn(float DeltaTime) {
	for(int i = 0; i < TeamPlayer.Num(); i++) {
		if(IsValid(TeamPlayer[i]) && TeamPlayer[i]->GetClass()->IsChildOf(APlayer_Controller::StaticClass())) {
			APlayer_Controller* aPlayerController = CastChecked<APlayer_Controller>(TeamPlayer[i]);
			if(aPlayerController->GetPawn() == nullptr && aPlayerController->RespawnTimer > GeneralHelper::PlayerRespawnTime) {
				ABase_DrivePawn* newCar = SpawnCar(aPlayerController, APlayer_DrivePawn::StaticClass());
			}
		}
	}
}

void ATeam::HandleAIRespawn(float DeltaTime) {
	AI_RespawnTimer += DeltaTime;

	AGameModeBase* gameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if(!gameModeBase) {
		return;
	}
	ABase_GameMode *gameMode = CastChecked<ABase_GameMode>(gameModeBase);
	UClass* aiClass = gameMode->GetAIPawnClass();

	// Respawn AIs until we hit the limit (if we dont have enough)
	for (int i = 0; TeamPlayer.Num() < gameMode->MaxCarsPerTeam; i++) {
		ABase_DrivePawn* car = SpawnCar(NULL, aiClass);
		TeamPlayer.Add(car->GetController());
	}

	// Dont allow respawn until respawn time is reached
	if (AI_RespawnTimer < gameMode->GetRespawnTime()) {
		return;
	}

	// Respawn "dead" AIs, one at a time
	for (int i = 0; i < TeamPlayer.Num(); i++) {
		AController *controller = TeamPlayer[i];

		if (!IsValid(controller)) {
			ABase_DrivePawn* car = SpawnCar(NULL, aiClass);
			TeamPlayer[i] = car->GetController();
			AI_RespawnTimer = 0;
			return;
		}
	}
}

void ATeam::AddPlayer(APlayer_Controller* controller) {
	controller->Team = this;
	int aiIndex = -1;
	// Try to find a index where we can insert the new controller
	for(int i = 0; i < TeamPlayer.Num(); i++) {
		if(!IsValid(TeamPlayer[i])) { // Controller in index is not valid anymore. Use the index
			aiIndex = i;
			break;
		}
		if(TeamPlayer[i]->GetClass()->IsChildOf(AAIController::StaticClass())) { // Choose a AI controller as backup (destroy it, if we dont have space)
			aiIndex = i;
		}
	}
	if(aiIndex == -1) { // Is there a index in the array we can use to add this player? 
		this->TeamPlayer.Add(controller); // If there is no space: Just add it
	} else {
		if(IsValid(TeamPlayer[aiIndex])) {
			// TODO: Destroy the pawn only (so "explosion" is triggered)
			ABase_DrivePawn* pawn = CastChecked<ABase_DrivePawn>(TeamPlayer[aiIndex]->GetPawn());
			pawn->DecreaseHealthByFloat(pawn->GetHealth());
		}
		TeamPlayer[aiIndex] = controller; // Replace the index with the new controller
	}
}

ABase_DrivePawn *ATeam::SpawnCar(AController *controller, UClass *driveClass) {
	if(!IsValid(this) || !HasAuthority()) {
		return NULL;
	}
	int tries = 0;
	UE_LOG(LogTemp, Display, TEXT("Trying to spawn car for %s for team %s"), controller == NULL ? TEXT("AI") : *controller->GetName(), *this->GetName());
	while (true) {
		FVector currentLocation = GetActorLocation();
		float randomX = this->GetActorLocation().X + UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
		float randomY = this->GetActorLocation().Y + UKismetMathLibrary::RandomFloatInRange(-1000, 1000);
		FVector lineTraceStart = FVector(randomX, randomY, GeneralHelper::LandscapeZStart);
		FVector lineTraceEnd = FVector(randomX, randomY, GeneralHelper::LandscapeZEnd);

		// Make line trace if we hit a landscape (prevent spawn over water for example)
		FHitResult outHit;
		TArray<AActor *> actorsToIgnore;
		bool traceSuccess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), lineTraceStart, lineTraceEnd, UEngineTypes::ConvertToTraceType(COLLISION_LANDSCAPE), false, actorsToIgnore, EDrawDebugTrace::None, outHit, true);

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
				}
				result->GetGunPawn()->SpawnDefaultController();
				result->SetTeam(this);
				result->SetSkeletonColor(Color);

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

int ATeam::GetNumberAIInTeam() {
	int result = 0;
	for(AController* controller : TeamPlayer) {
		if(controller->GetClass()->IsChildOf(AAIController::StaticClass())) {
			result++;
		}
	}
	return result;
}
int ATeam::GetNumberPlayerInTeam() {
	int result = 0;
	for(AController* controller : TeamPlayer) {
		if(controller->GetClass()->IsChildOf(APlayerController::StaticClass())) {
			result++;
		}
	}
	return result;
}
bool ATeam::CanSupportAnotherPlayer() {
	return GetNumberAIInTeam() > 0;
}

void ATeam::IncreasePoints(int amount) {
	Points += amount;
	UE_LOG(LogTemp, Warning, TEXT("%s scored %i points. Now at %i points!"), *Name, amount, Points);
}

TArray<ABase_DrivePawn*> ATeam::GetTeamPlayerAsDrivePawns() {
	TArray<ABase_DrivePawn*> result;
	for(AController* controller : TeamPlayer) {
		if(IsValid(controller) && IsValid(controller->GetPawn())) {
			result.Add(CastChecked<ABase_DrivePawn>(controller->GetPawn()));
		}
	}
	return result;
}