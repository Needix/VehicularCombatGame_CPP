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

class APlayer_Controller;

UCLASS()
class VEHICLECOMBATGAMECPP_API ATeam : public AActor {
	GENERATED_BODY()

	// Variables
  private:
	float DeltaSeconds;
	float AI_RespawnTimer;

	UPROPERTY(ReplicatedUsing=OnRep_SetupPropertyName)
	FString Name;
	UPROPERTY(ReplicatedUsing=OnRep_SetupPropertyId)
	int Id;
	UPROPERTY(ReplicatedUsing=OnRep_SetupPropertyColor)
	FVector Color;
	int Points;

	UParticleSystemComponent* baseParticleSystemComponent;

  public:
	UPROPERTY()
	TArray<AController*> TeamPlayer;

	// Functions
  private:
    UFUNCTION()
	void OnRep_SetupPropertyName();
    UFUNCTION()
	void OnRep_SetupPropertyId();
    UFUNCTION()
	void OnRep_SetupPropertyColor();
    UFUNCTION()
	void OnRep_SetupProperty();

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
	
	int GetNumberAIInTeam();
	int GetNumberPlayerInTeam();
	bool CanSupportAnotherPlayer();

	void IncreasePoints(int amount);
	void DecreasePoints(int amount) { Points -= amount; }

	void AddPlayer(APlayer_Controller* controller);

	ABase_DrivePawn *SpawnCar(AController *controller, UClass *driveClass);
	
	UFUNCTION(BlueprintCallable, Category = Getter)
	FString GetName() { return Name; }
	UFUNCTION(BlueprintCallable, Category = Getter)
	int GetPoints() { return Points; }
	UFUNCTION(BlueprintCallable, Category = Getter)
	FVector GetColor() { return Color; }

	TArray<ABase_DrivePawn*> GetTeamPlayerAsDrivePawns();
};
