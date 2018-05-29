// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_Item.h"

#include "ConstructorHelpers.h"
#include "Engine/Texture2D.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "Base/Base_GunPawn.h"
#include "Base/Base_DrivePawn.h"

// Sets default values
ABase_Item::ABase_Item() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponents();
	SetupEvents();
}

void ABase_Item::SetupComponents() {
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/VehicleAdv/Materials/Fixed_Template_Master.Fixed_Template_Master'"));
	MaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, Material.Object);
	MaterialInstance->SetScalarParameterValue("Metallic", 0);
	MaterialInstance->SetScalarParameterValue("Roughness", 0.25);

	ConstructorHelpers::FObjectFinder<UStaticMesh> cube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> cylinder(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> sphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	UStaticMeshComponent* baseCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseCylinder"));
	baseCylinder->SetStaticMesh(cylinder.Object);
	baseCylinder->SetRelativeScale3D(FVector(0.2, 0.2, 0.2));
	baseCylinder->bGenerateOverlapEvents = true;
	baseCylinder->SetMaterial(0, MaterialInstance);
	baseCylinder->SetupAttachment(RootComponent);
	baseCylinder->SetCollisionProfileName(TEXT("OverlapAll"));
	//baseCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UStaticMeshComponent* sphere1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere1"));
	sphere1->SetStaticMesh(sphere.Object);
	sphere1->SetRelativeLocation(FVector(0, 0, 50));
	sphere1->SetMaterial(0, MaterialInstance);
	sphere1->SetupAttachment(baseCylinder);
	sphere1->SetCollisionProfileName(TEXT("OverlapAll"));

	UStaticMeshComponent* sphere2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere2"));
	sphere2->SetStaticMesh(sphere.Object);
	sphere2->SetRelativeLocation(FVector(0, 0, -50));
	sphere2->SetMaterial(0, MaterialInstance);
	sphere2->SetupAttachment(baseCylinder);
	sphere2->SetCollisionProfileName(TEXT("OverlapAll"));

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetRelativeLocation(FVector(0, 0, 100));
	ParticleSystem->SetMaterial(0, MaterialInstance);
	ParticleSystem->SetupAttachment(baseCylinder);

	IconBillboard = CreateDefaultSubobject<UBillboardComponent>("Icon");
	IconBillboard->SetRelativeLocation(FVector(0, 0, 1800));
	IconBillboard->SetRelativeScale3D(FVector(5, 5, 5));
	IconBillboard->SetHiddenInGame(false);
	IconBillboard->SetupAttachment(baseCylinder);
}
void ABase_Item::SetupEvents() {
	UBoxComponent* boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	boxCollision->SetRelativeLocation(FVector(0, 0, 0));
	boxCollision->SetRelativeScale3D(FVector(1, 1, 1));
	boxCollision->SetupAttachment(RootComponent);

	FScriptDelegate componentOverlapDelegate;
	componentOverlapDelegate.BindUFunction(this, "BoxCollisionComponentOverlap");
	boxCollision->OnComponentBeginOverlap.AddUnique(componentOverlapDelegate);
}

void ABase_Item::Use() {
	if(Ammo <= 0) {
		Destroy();
	}
}

void ABase_Item::BoxCollisionComponentOverlap(class AActor* myActor, class AActor* otherActor) {
	if(PickedUp) {
		return;
	}
	if(otherActor->GetClass()->IsChildOf(ABase_DrivePawn::StaticClass())) {
		ABase_DrivePawn* pawn = CastChecked<ABase_DrivePawn>(otherActor);
		pawn->GetGunPawn()->SetItem(this);
		this->SetOwner(pawn->GetGunPawn());
		RootComponent->SetVisibility(false, true);
		PickedUp = true;
	}
}

// Called when the game starts or when spawned
void ABase_Item::BeginPlay() {
	Super::BeginPlay();

	IconBillboard->SetSprite(ItemIcon);
}

// Called every frame
void ABase_Item::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}