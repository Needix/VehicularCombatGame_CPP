// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelBoundary.h"
#include "Engine/Engine.h"

// Sets default values
ALevelBoundary::ALevelBoundary() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
}

// Called when the game starts or when spawned
void ALevelBoundary::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ALevelBoundary::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
