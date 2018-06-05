// Fill out your copyright notice in the Description page of Project Settings.

#include "MinigunBullet.h"

#include "ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Base/Base_DrivePawn.h"
#include "Base/Base_GunPawn.h"


// Sets default values
AMinigunBullet::AMinigunBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponents();

}

void AMinigunBullet::SetupComponents() {
	ConstructorHelpers::FObjectFinder<UStaticMesh> sphere(TEXT("StaticMesh'/Game/Meshes/MachineGunBullet.MachineGunBullet'"));

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	RootComponent = Bullet = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet"));
	Bullet->SetSimulatePhysics(false);
	Bullet->SetRelativeLocation(FVector(0, 0, 0));
	Bullet->SetRelativeScale3D(FVector(0.01, 0.01, 0.02));
	//Bullet->SetCollisionProfileName("OverlapAll");
	//Bullet->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Bullet->SetStaticMesh(sphere.Object);
	Bullet->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystem(TEXT("ParticleSystem'/Game/VehicularCombatGame/ParticleEffects/MachineGunBulletParticleSystem.MachineGunBulletParticleSystem'"));
	
	particleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	particleSystemComponent->SetTemplate(particleSystem.Object);
	particleSystemComponent->SetupAttachment(Bullet);

	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	boxCollision->SetRelativeScale3D(FVector(0.3, 0.3, 0.3));
	boxCollision->SetupAttachment(Bullet);

	FScriptDelegate onComponentHit;
	onComponentHit.BindUFunction(this, "OnComponentHitFunction");
	Bullet->OnComponentHit.AddUnique(onComponentHit);

	FScriptDelegate overlapDelegate1;
	overlapDelegate1.BindUFunction(this, "HandleOverlapStartEvent");
	boxCollision->OnComponentBeginOverlap.AddUnique(overlapDelegate1);
}

// Called when the game starts or when spawned
void AMinigunBullet::BeginPlay() {
	Super::BeginPlay();
	
	FRotator currentRotation = Bullet->GetComponentRotation();
	FVector speed = -1 * UKismetMathLibrary::GetUpVector(currentRotation) * InitialSpeed * 100; // Rotate and multiply by 100 for centimeters
	Bullet->SetSimulatePhysics(true);
	Bullet->AddImpulse(speed, NAME_None, true);
	IsFlying = true;
	Bullet->SetNotifyRigidBodyCollision(true);
}

void AMinigunBullet::OnComponentHitFunction(UPrimitiveComponent *hitComponent, AActor *otherActor, UPrimitiveComponent *otherComponent, FVector normalImpulse, FHitResult &hit) {
	HandleOverlapStartEvent(this, otherActor);
}

void AMinigunBullet::HandleOverlapStartEvent(class AActor *myActor, class AActor *otherActor) {
	HandleHit(otherActor);
}

void AMinigunBullet::HandleHit(class AActor* otherActor) {
	ABase_Item* owner = CastChecked<ABase_Item>(GetOwner());
	if(this == otherActor || otherActor->GetClass()->IsChildOf(AMinigunBullet::StaticClass()) || otherActor == owner->GetGunPawn() || otherActor == owner->GetGunPawn()->GetDrivePawn()) {
		return;
	}

	// TODO: Add check for ATeam etz. These objects will block the shots

	if(otherActor->GetClass()->IsChildOf(ABase_GunPawn::StaticClass()) || otherActor->GetClass()->IsChildOf(ABase_DrivePawn::StaticClass())) {
		ABase_DrivePawn* drivePawn;
		if(otherActor->GetClass()->IsChildOf(ABase_GunPawn::StaticClass())) {
			drivePawn = CastChecked<ABase_GunPawn>(otherActor)->GetDrivePawn();
		} else {
			drivePawn = CastChecked<ABase_DrivePawn>(otherActor);
		}
		drivePawn->DecreaseHealthByFloat(Damage);
	}
	
	UE_LOG(LogTemp, Verbose, TEXT("MinigunBullet %s hit %s"), *this->GetName(), *otherActor->GetName());
	// TODO: Spawn hit particle effect on location
	Destroy();
}

// Called every frame
void AMinigunBullet::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if(!IsValid(this)) {
		return;
	}

	if(IsFlying) {
		SecondsBeforeDespawnTimer += DeltaTime;
	}
	if(SecondsBeforeDespawnTimer > SecondsBeforeDespawnTime) {
		Destroy();
	}
}

