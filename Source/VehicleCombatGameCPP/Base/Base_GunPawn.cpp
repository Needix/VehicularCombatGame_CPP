// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_GunPawn.h"

#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "GameFramework/GameMode.h"

#include "Base/Base_GameMode.h"

#include "Components/StaticMeshComponent.h"

const FName ABase_GunPawn::LookUpBinding("LookUp");
const FName ABase_GunPawn::LookRightBinding("LookRight");

// Sets default values
ABase_GunPawn::ABase_GunPawn() {
	if(HasAuthority() && IsValid(GetWorld())) {
		AGameModeBase* gm = GetWorld()->GetAuthGameMode();
		if(IsValid(gm) && gm->GetClass()->IsChildOf(ABase_GameMode::StaticClass())) {
			AIControllerClass = CastChecked<ABase_GameMode>(gm)->GetAIControllerClass();
		}
	}

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponents();
}

void ABase_GunPawn::SetupComponents() {
	ConstructorHelpers::FObjectFinder<UStaticMesh> cube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> cylinder(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/VehicleAdv/Materials/Fixed_Template_Master.Fixed_Template_Master'"));
	SkeletonMeshMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, Material.Object);
	SkeletonMeshMaterialInstance->SetScalarParameterValue("Metallic", 0);
	SkeletonMeshMaterialInstance->SetScalarParameterValue("Roughness", 0.25);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	UStaticMeshComponent* base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	base->SetStaticMesh(cube.Object);
	base->SetRelativeLocation(FVector(0, 0, 40));
	base->SetRelativeScale3D(FVector(0.4, 0.4, 0.01));
	base->SetMaterial(0, SkeletonMeshMaterialInstance);
	base->SetupAttachment(RootComponent);

	YawBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotatingCylinder"));
	YawBase->SetStaticMesh(cylinder.Object);
	YawBase->SetRelativeLocation(FVector(0, 0, 100));
	YawBase->SetRelativeScale3D(FVector(1, 1, 1));
	YawBase->SetMaterial(0, SkeletonMeshMaterialInstance);
	YawBase->SetupAttachment(base);

	// Scene 1
	USceneComponent* sceneComponent1 = CreateDefaultSubobject<USceneComponent>(TEXT("Scene1"));
	sceneComponent1->SetRelativeLocation(FVector(0, 0, 2000));
	sceneComponent1->SetRelativeScale3D(FVector(0.1, 0.1, 40));
	sceneComponent1->SetupAttachment(YawBase);
	
	UStaticMeshComponent* cube1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube1"));
	cube1->SetStaticMesh(cube.Object);
	cube1->SetRelativeLocation(FVector(0, 400, 0));
	cube1->SetRelativeScale3D(FVector(1, 1, 1));
	cube1->SetMaterial(0, SkeletonMeshMaterialInstance);
	cube1->SetupAttachment(sceneComponent1);
	
	UStaticMeshComponent* cube2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube2"));
	cube2->SetStaticMesh(cube.Object);
	cube2->SetRelativeLocation(FVector(0, -400, 0));
	cube2->SetRelativeScale3D(FVector(1, 1, 1));
	base->SetMaterial(0, SkeletonMeshMaterialInstance);
	cube2->SetupAttachment(sceneComponent1);
	
	// Scene 2
	USceneComponent* sceneComponent2 = CreateDefaultSubobject<USceneComponent>(TEXT("Scene2"));
	sceneComponent2->SetRelativeLocation(FVector(0, 0, 3500));
	sceneComponent2->SetRelativeRotation(FRotator(0, 90, 0));
	sceneComponent2->SetRelativeScale3D(FVector(1, 1, 1));
	sceneComponent2->SetupAttachment(YawBase);

	UStaticMeshComponent* barrelBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelBase"));
	barrelBase->SetStaticMesh(cylinder.Object);
	barrelBase->SetRelativeLocation(FVector(0, 0, 35));
	barrelBase->SetRelativeRotation(FRotator(90, 0, 0));
	barrelBase->SetRelativeScale3D(FVector(0.1, 0.1, 30));
	barrelBase->SetMaterial(0, SkeletonMeshMaterialInstance);
	barrelBase->SetupAttachment(sceneComponent2);

	PitchBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel"));
	PitchBase->SetStaticMesh(cylinder.Object);
	PitchBase->SetRelativeLocation(FVector(0, -40, 35));
	PitchBase->SetRelativeRotation(FRotator(90, 0, 90));
	PitchBase->SetRelativeScale3D(FVector(0.2, 0.2, 50));
	PitchBase->SetMaterial(0, SkeletonMeshMaterialInstance);
	PitchBase->SetupAttachment(sceneComponent2);

	UStaticMeshComponent* barrelInsde = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelInside"));
	barrelInsde->SetStaticMesh(cylinder.Object);
	barrelInsde->SetRelativeLocation(FVector(0, 0, -20));
	barrelInsde->SetRelativeRotation(FRotator(0, 0, 0));
	barrelInsde->SetRelativeScale3D(FVector(0.5, 0.5, 0.9));
	barrelInsde->SetMaterial(0, SkeletonMeshMaterialInstance);
	barrelInsde->SetupAttachment(PitchBase);
}

// Called when the game starts or when spawned
void ABase_GunPawn::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ABase_GunPawn::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	HandleMovement(deltaTime);
	HandlePrimaryFire(deltaTime);
}

void ABase_GunPawn::HandleMovement(float deltaTime) {
	if(IsValid(InputComponent)) {
		YawBase->AddRelativeRotation(FRotator(0, deltaTime * InputComponent->GetAxisValue(LookUpBinding), 0));
		PitchBase->AddRelativeRotation(FRotator(deltaTime * InputComponent->GetAxisValue(LookRightBinding), 0, 0));
	}
}
void ABase_GunPawn::HandlePrimaryFire(float deltaTime) {
	
}


// Called to bind functionality to input
void ABase_GunPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(LookUpBinding);
	PlayerInputComponent->BindAxis(LookRightBinding);
	
	PlayerInputComponent->BindAction("FirePrimaryWeapon", IE_Pressed, this, &ABase_GunPawn::OnPrimaryFirePressed);
	PlayerInputComponent->BindAction("FirePrimaryWeapon", IE_Released, this, &ABase_GunPawn::OnPrimaryFireReleased);
	PlayerInputComponent->BindAction("SwitchRole", IE_Released, this, &ABase_GunPawn::OnSwitchRole);
}

void ABase_GunPawn::OnSwitchRole() {
	
}

void ABase_GunPawn::SetColor(FVector color) {
	SkeletonMeshMaterialInstance->SetVectorParameterValue("DiffuseColor", FLinearColor(color.X, color.Y, color.Z, 0));
}
