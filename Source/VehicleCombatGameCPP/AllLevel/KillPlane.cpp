// Fill out your copyright notice in the Description page of Project Settings.

#include "KillPlane.h"

// Sets default values
AKillPlane::AKillPlane() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitCustomComponents();
}

void AKillPlane::InitCustomComponents() {
	UStaticMeshComponent *killPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KillPlane"));
	RootComponent = killPlaneComponent;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> killPlane(TEXT("/Game/Meshes/KillPlane.KillPlane"));
	if (killPlane.Succeeded()) {
		killPlaneComponent->SetStaticMesh(killPlane.Object);
		killPlaneComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		killPlaneComponent->SetWorldScale3D(FVector(2000, 2000, 10));
	}

	killPlaneComponent->SetMobility(EComponentMobility::Static);
	killPlaneComponent->SetCollisionProfileName("Trigger");
	killPlaneComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
}

// Called when the game starts or when spawned
void AKillPlane::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AKillPlane::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
