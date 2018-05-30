// Fill out your copyright notice in the Description page of Project Settings.

#include "CTF_Flag.h"

#include "UObject/ConstructorHelpers.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

#include "GameModes/Team.h"
#include "Base/Base_DrivePawn.h"

// Sets default values
ACTF_Flag::ACTF_Flag() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> flagMesh(TEXT("/Game/Meshes/CTF_Flag.CTF_Flag"));
	UStaticMeshComponent* comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	comp->SetStaticMesh(flagMesh.Object);
	RootComponent = comp;
	//RootComponent->Set
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystem(TEXT("/Game/VehicularCombatGame/ParticleEffects/CTF_ParticleSystem.CTF_ParticleSystem"));
	UParticleSystemComponent* particleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	particleSystemComponent->SetTemplate(particleSystem.Object);
	particleSystemComponent->SetupAttachment(RootComponent);

	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	boxCollision->SetRelativeLocation(FVector(0, 0, 150));
	boxCollision->SetRelativeScale3D(FVector(3, 3, 7));
	boxCollision->SetupAttachment(RootComponent);

	FScriptDelegate componentOverlapDelegate;
	componentOverlapDelegate.BindUFunction(this, "BoxCollisionComponentOverlap");
	boxCollision->OnComponentBeginOverlap.AddUnique(componentOverlapDelegate);

	FScriptDelegate overlapDelegate2;
    overlapDelegate2.BindUFunction(this, "HandleOverlapEndEvent");
	OnActorEndOverlap.AddUnique(overlapDelegate2);
}

// Called when the game starts or when spawned
void ACTF_Flag::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ACTF_Flag::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	ReattachTimer += DeltaTime;
	if(ReattachTimer > 1) {
		boxCollision->bGenerateOverlapEvents = true;
	} else {
		boxCollision->bGenerateOverlapEvents = false;
	}
}

void ACTF_Flag::BoxCollisionComponentOverlap(class AActor* myActor, class AActor* otherActor) {
	UClass* otherActorClass = otherActor->GetClass();
	if(otherActorClass->IsChildOf(ABase_DrivePawn::StaticClass())) { // Potential Player pick up
		if(DrivePawn == myActor && ReattachTimer < 1) {
			return;
		}

		DrivePawn = CastChecked<ABase_DrivePawn>(otherActor);
		EAttachmentRule attachmentRule = EAttachmentRule::SnapToTarget;
		FAttachmentTransformRules attachmentRules = FAttachmentTransformRules(attachmentRule, true);
		UE_LOG(LogTemp, Warning, TEXT("Attaching %s to %s"), *this->GetName(), *DrivePawn->GetName());
		ReattachTimer = 0;
		boxCollision->bGenerateOverlapEvents = false;
		this->AttachToActor(DrivePawn, attachmentRules);
	} else if(otherActorClass->IsChildOf(ATeam::StaticClass())) { // Delivered to base?
		if(true) return;
		if(!IsValid(DrivePawn)) {
			return;
		}
		ATeam* team = CastChecked<ATeam>(otherActor);
		ABase_DrivePawn* driver = CastChecked<ABase_DrivePawn>(GetOwner());
		if(driver->GetTeam() == team) {
			team->IncreasePoints(1);
			Destroy();
		}
	}
}
