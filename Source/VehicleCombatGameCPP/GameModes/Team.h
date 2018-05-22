// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Base/Base_DrivePawn.h"
//#include "Controller.h"
#include "Team.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API ATeam : public AActor {
	GENERATED_BODY()

	// Variables
  private:
	float DeltaSeconds;
	float AI_RespawnTimer;

  public:
	UPROPERTY()
	TArray<AController *> TeamPlayer;

	// Functions
  public:
	// Sets default values for this actor's properties
	ATeam();

  private:
	void HandleAIRespawn(float DeltaTime);

  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ABase_DrivePawn *SpawnCar(AController *controller, UClass *driveClass);
	ABase_DrivePawn *SpawnCar(AController *controller, UClass *driveClass, int controllerIndex);
};
