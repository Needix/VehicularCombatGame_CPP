// Fill out your copyright notice in the Description page of Project Settings.

#include "KillPlane.h"


// Sets default values
AKillPlane::AKillPlane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKillPlane::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKillPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

