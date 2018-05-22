// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "KillPlane.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API AKillPlane : public AActor {
	GENERATED_BODY()

	/*UPROPERTY(EditDefaultsOnly, Category = "Static Mesh")
	UStaticMeshComponent* ourMesh;*/

  public:
	// Sets default values for this actor's properties
	AKillPlane();

  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  private:
	void InitCustomComponents();
};
