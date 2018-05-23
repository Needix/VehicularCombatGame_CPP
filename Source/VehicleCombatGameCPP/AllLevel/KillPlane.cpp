// Fill out your copyright notice in the Description page of Project Settings.

#include "KillPlane.h"

#include "Components/BoxComponent.h"

// Sets default values
AKillPlane::AKillPlane() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitCustomComponents();
}

void AKillPlane::InitCustomComponents() {
	float x = 2000;
	float y = 2000;
	float z = 1;

	UStaticMeshComponent *killPlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KillPlane"));
	RootComponent = killPlaneComponent;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> killPlane(TEXT("/Game/Meshes/KillPlane.KillPlane"));
	killPlaneComponent->SetStaticMesh(killPlane.Object);
	killPlaneComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	killPlaneComponent->SetWorldScale3D(FVector(x, y, z));
	killPlaneComponent->SetMobility(EComponentMobility::Static);
	killPlaneComponent->SetCollisionProfileName("Trigger");
	killPlaneComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	
	UBoxComponent* boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	boxCollision->SetWorldScale3D(FVector(1, 1, 1));
	boxCollision->SetBoxExtent(FVector(32, 32, 1), true);
	boxCollision->SetRelativeLocation(FVector(0, 0, -1));
	boxCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AKillPlane::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AKillPlane::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
