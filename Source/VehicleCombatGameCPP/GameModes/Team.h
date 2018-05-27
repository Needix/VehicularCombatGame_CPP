// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine Header
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Component Header
#include "Particles/ParticleSystemComponent.h"

// Custom Header
#include "Base/Base_DrivePawn.h"

#include "Team.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API ATeam : public AActor {
	GENERATED_BODY()

	// Variables
  private:
	float DeltaSeconds;
	float AI_RespawnTimer;

	FString Name;
	int Id;
	FVector Color;
	int Points;

	UParticleSystemComponent* baseParticleSystemComponent;

  public:
	UPROPERTY()
	TArray<AController*> TeamPlayer;

	// Functions
  private:
	void HandleAIRespawn(float DeltaTime);
	void HandlePlayerRespawn(float Delta);

  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  public:
	ATeam();

	void Setup(FString name, int id, FVector color);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void IncreasePoints(int amount);
	void DecreasePoints(int amount) { Points -= amount; }

	ABase_DrivePawn *SpawnCar(AController *controller, UClass *driveClass);
	ABase_DrivePawn *SpawnCar(AController *controller, UClass *driveClass, int controllerIndex);
	
	UFUNCTION(BlueprintCallable, Category = Getter)
	FString GetName() { return Name; }
	UFUNCTION(BlueprintCallable, Category = Getter)
	int GetPoints() { return Points; }
	UFUNCTION(BlueprintCallable, Category = Getter)
	FVector GetColor() { return Color; }
};
