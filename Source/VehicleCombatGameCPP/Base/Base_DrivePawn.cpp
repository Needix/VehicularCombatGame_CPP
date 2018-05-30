// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Base_DrivePawn.h"
// Engine Header
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UnrealNetwork.h"

// Components Header
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/GameMode.h"

// Custom Header
#include "VehicleCombatGameCPPWheelFront.h"
#include "VehicleCombatGameCPPWheelRear.h"
#include "VehicleCombatGameCPPHud.h"
#include "AI/AI_Controller.h"
#include "AllLevel/KillPlane.h"
#include "Base/Base_GameMode.h"
#include "GameModes/Team.h"
#include "Helper/GeneralHelper.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED


#include "Base_GunPawn.h"

const FName ABase_DrivePawn::EngineAudioRPM("RPM");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ABase_DrivePawn::ABase_DrivePawn() {
	if(HasAuthority() && IsValid(GetWorld())) {
		AGameModeBase* gm = GetWorld()->GetAuthGameMode();
		if(IsValid(gm) && gm->GetClass()->IsChildOf(ABase_GameMode::StaticClass())) {
			AIControllerClass = CastChecked<ABase_GameMode>(gm)->GetAIControllerClass();
		}
	}

	bReplicates = true;
	bAlwaysRelevant = true;

	InitializeBasicComponents();

	// Setup friction materials
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/Slippery.Slippery"));
	SlipperyMaterial = SlipperyMat.Object;

	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> NonSlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/NonSlippery.NonSlippery"));
	NonSlipperyMaterial = NonSlipperyMat.Object;

	InitializeEventDelegates();
	InitializeCar();
	InitializeOtherComponents();
	InitializeParticleSystems();
	InitializeSoundCues();
	InitializeFlipCarTimeline();

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bIsLowFriction = false;
	bInReverseGear = false;
}

void ABase_DrivePawn::BeginPlay() {
	Super::BeginPlay();

	InitializeTurretSpawn();

	// First disable both speed/gear displays
	bInCarCameraActive = false;
	InCarSpeed->SetVisibility(bInCarCameraActive);
	InCarGear->SetVisibility(bInCarCameraActive);

	// Start an engine sound playing
	EngineSoundComponent->Play();
}

void ABase_DrivePawn::InitializeEventDelegates() {
	GetMesh()->bGenerateOverlapEvents = true;

	FScriptDelegate overlapDelegate1;
	overlapDelegate1.BindUFunction(this, "HandleOverlapStartEvent");
	OnActorBeginOverlap.AddUnique(overlapDelegate1);

	FScriptDelegate overlapDelegate2;
	overlapDelegate2.BindUFunction(this, "HandleOverlapEndEvent");
	OnActorEndOverlap.AddUnique(overlapDelegate2);

	FScriptDelegate hitDelegate;
	hitDelegate.BindUFunction(this, "HandleHitEvent");
	GetMesh()->OnComponentHit.AddUnique(hitDelegate);
}
void ABase_DrivePawn::InitializeBasicComponents() {
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/VehicleAdv/Vehicle/Vehicle_SkelMesh.Vehicle_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/VehicleAdv/Materials/Fixed_Template_Master.Fixed_Template_Master'"));
	SkeletonMeshMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, Material.Object);
	SkeletonMeshMaterialInstance->SetScalarParameterValue("Metallic", 0);
	SkeletonMeshMaterialInstance->SetScalarParameterValue("Roughness", 0.25);
	GetMesh()->SetMaterial(0, SkeletonMeshMaterialInstance);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/VehicleAdv/Vehicle/VehicleAnimationBlueprint"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	GetMesh()->bGenerateOverlapEvents = true;
}
void ABase_DrivePawn::InitializeParticleSystems() {
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExhaustParticleSystem(TEXT("ParticleSystem'/Game/VehicularCombatGame/ParticleEffects/Fire.Fire'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> SmokeParticles(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Smoke.P_Smoke'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FireParticles(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionParticles(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));

	SmokeParticleSystem = SmokeParticles.Object;
	FireParticleSystem = FireParticles.Object;
	ExplosionParticleSystem = ExplosionParticles.Object;

	ExhaustParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExhaustParticles"));
	ExhaustParticles->SetRelativeLocation(FVector(-78.8f, -5.8f, 26.6f));
	ExhaustParticles->SetTemplate(ExhaustParticleSystem.Object);
	ExhaustParticles->SetupAttachment(GetMesh());

	HealthParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HealthParticles"));
	HealthParticles->bAutoActivate = false;
	HealthParticles->SetupAttachment(GetMesh());
}
void ABase_DrivePawn::InitializeSoundCues() {
	static ConstructorHelpers::FObjectFinder<USoundCue> SmokeSound(TEXT("SoundCue'/Game/StarterContent/Audio/Smoke01_Cue.Smoke01_Cue'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> FireSound(TEXT("SoundCue'/Game/StarterContent/Audio/Fire01_Cue.Fire01_Cue'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> ExplosionSound(TEXT("SoundCue'/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue'"));

	SmokeSoundCue = SmokeSound.Object;
	FireSoundCue = FireSound.Object;
	ExplosionSoundCue = ExplosionSound.Object;

	HealthSound = CreateDefaultSubobject<UAudioComponent>(TEXT("HealthSound"));
	HealthSound->bAutoActivate = false;
	HealthSound->SetupAttachment(GetMesh());
}
void ABase_DrivePawn::InitializeCar() {
	UWheeledVehicleMovementComponent4W *Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);
	// Wheels/Tyres
	// Setup the wheels
	Vehicle4W->WheelSetups[0].WheelClass = UVehicleCombatGameCPPWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("PhysWheel_FL");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UVehicleCombatGameCPPWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("PhysWheel_FR");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UVehicleCombatGameCPPWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("PhysWheel_BL");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -8.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UVehicleCombatGameCPPWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("PhysWheel_BR");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 8.f, 0.f);

	// Adjust the tire loading
	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	// Engine
	// Torque setup
	Vehicle4W->MaxEngineRPM = 5700.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	// Adjust the steering
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);

	// Transmission
	// We want 4wd
	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;

	// Drive the front wheels a little more than the rear
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	Vehicle4W->bDeprecatedSpringOffsetMode = true; // TODO

	// Physics settings
	// Adjust the center of mass - the buggy is quite low
	UPrimitiveComponent *UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive) {
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(8.0f, 0.0f, 0.0f);
	}

	// Set the inertia scale. This controls how the mass of the vehicle is distributed.
	Vehicle4W->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);
}
void ABase_DrivePawn::InitializeOtherComponents() {
	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 125.0f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	// Create the chase camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeLocation(FVector(-125.0, 0.0f, 0.0f));
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component
	InternalCameraOrigin = FVector(-34.0f, -10.0f, 50.0f);
	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	// In car HUD
	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InCarSpeed->SetRelativeLocation(FVector(35.0f, -6.0f, 20.0f));
	InCarSpeed->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	InCarGear->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
	InCarGear->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	InCarGear->SetupAttachment(GetMesh());

	// Setup the audio component and allocate it a sound cue
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue"));
	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	EngineSoundComponent->SetSound(SoundCue.Object);
	EngineSoundComponent->SetupAttachment(GetMesh());

	// Create the topdown camera component
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopdownCamera"));
	TopDownCamera->SetupAttachment(RootComponent);
	TopDownCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 10000.0f));
	TopDownCamera->SetRelativeRotation(FRotator(270.0f, 00.0f, 0.0f));
	TopDownCamera->bUsePawnControlRotation = false;
	TopDownCamera->FieldOfView = 90.f;
}
void ABase_DrivePawn::InitializeFlipCarTimeline() {
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("CurveFloat'/Game/VehicularCombatGame/Curves/Linear.Linear'"));

	FlipCarTimeline = NewObject<UTimelineComponent>(this, FName("FlipCarTimeline"));
	FlipCarTimeline->SetPropertySetObject(this);

	FOnTimelineFloat callback;
	FOnTimelineEventStatic onFinishedCallback;

	callback.BindUFunction(this, FName{TEXT("FlipCarTimelineCallback")});
	FlipCarTimeline->AddInterpFloat(Curve.Object, callback);

	onFinishedCallback.BindUFunction(this, FName{TEXT("FlipCarTimelineFinishedCallback")});
	FlipCarTimeline->SetTimelineFinishedFunc(onFinishedCallback);

	FlipCarTimeline->RegisterComponent();
}
void ABase_DrivePawn::InitializeTurretSpawn() {
	FActorSpawnParameters spawnParameters = FActorSpawnParameters();
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GunPawn = GetWorld()->SpawnActor<ABase_GunPawn>(ABase_GunPawn::StaticClass(), FTransform(FVector(this->GetActorLocation().X, this->GetActorLocation().Y, this->GetActorLocation().Z + 5000)), spawnParameters);
	FAttachmentTransformRules transformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	GunPawn->AttachToActor(this, transformRules, "TurretSocket");
	GunPawn->SetDrivePawn(this);
}

void ABase_DrivePawn::Tick(float Delta) {
	Super::Tick(Delta);

	CurrentDeltaSeconds = Delta;
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;

	UpdateHealth();
	UpdatePhysicsMaterial();
	UpdateHUDStrings();
	UpdateInCarHUD();
	UpdateSound();
	FlipCar();
	UpdateCarColor();
}
void ABase_DrivePawn::UpdateSound() {
	// Pass the engine RPM to the sound component
	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComponent->SetFloatParameter(EngineAudioRPM, GetVehicleMovement()->GetEngineRotationSpeed() * RPMToAudioScale);
}
void ABase_DrivePawn::UpdateHUDStrings() {
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);
	int32 Gear = GetVehicleMovement()->GetCurrentGear();

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));

	if (bInReverseGear) {
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	} else {
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}
}
void ABase_DrivePawn::UpdateInCarHUD() {
	APlayerController *PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr)) {
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);

		if (bInReverseGear) {
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		} else {
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
	}
}
void ABase_DrivePawn::UpdatePhysicsMaterial() {
	if (GetActorUpVector().Z < 0) {
		if (bIsLowFriction) {
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			bIsLowFriction = false;
		} else {
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			bIsLowFriction = true;
		}
	}
}
void ABase_DrivePawn::UpdateCarColor() {
	if(IsValid(Team)) {
		SetSkeletonColor(Team->GetColor());
	}
}

void ABase_DrivePawn::HandleOverlapStartEvent(class AActor *myActor, class AActor *otherActor) {
	if (otherActor->GetClass()->IsChildOf(AKillPlane::StaticClass())) {
		AKillPlane *killPlane = CastChecked<AKillPlane>(otherActor);
		IsCollidingWithKillPlane = true;
	}
}
void ABase_DrivePawn::HandleOverlapEndEvent(class AActor *myActor, class AActor *otherActor) {
	if (otherActor->GetClass()->IsChildOf(AKillPlane::StaticClass())) {
		AKillPlane *killPlane = CastChecked<AKillPlane>(otherActor);
		IsCollidingWithKillPlane = killPlane->GetActorLocation().Z > this->GetActorLocation().Z;
	}
}

void ABase_DrivePawn::HandleHitEvent(UPrimitiveComponent *hitComponent, AActor *otherActor, UPrimitiveComponent *otherComponent, FVector normalImpulse, FHitResult &hit) {
	if (otherActor->GetClass()->IsChildOf(ABase_DrivePawn::StaticClass())) {
		DecreaseHealthByFloat(normalImpulse.Size() * GeneralHelper::CarCollisionForceMultiplier);
	}
}

void ABase_DrivePawn::DecreaseHealthByTime(float deltaSecondsMultiplicator) {
	DecreaseHealthByFloat(CurrentDeltaSeconds * deltaSecondsMultiplicator);
}
void ABase_DrivePawn::DecreaseHealthByFloat(float health) {
	Health -= health;

	if (Health <= 0) {
		Health = 0;
	}
}
void ABase_DrivePawn::IncreaseHealthByFloat(float amount) {
	Health += amount;

	if(Health >= 100) {
		Health = 100;
	}
}
void ABase_DrivePawn::UpdateHealth() {
	if (IsCollidingWithKillPlane) {
		DecreaseHealthByTime(10);
	}

	HealthStageEnum newStage;

	if (Health <= 0) {
		newStage = HealthStageEnum::Explosion;
	} else if (Health <= 10) {
		newStage = HealthStageEnum::Fire;
	} else if (Health <= 30) {
		newStage = HealthStageEnum::Smoke;
	} else {
		newStage = HealthStageEnum::Normal;
	}

	if (newStage != HealthStage) {
		HealthStage = newStage;

		switch (HealthStage) {
			case HealthStageEnum::Normal:
				HealthParticles->DeactivateSystem();

				HealthSound->Deactivate();
				break;
			case HealthStageEnum::Smoke:
				HealthParticles->SetTemplate(SmokeParticleSystem);
				HealthParticles->ActivateSystem();

				HealthSound->SetSound(SmokeSoundCue);
				HealthSound->Activate();
				break;
			case HealthStageEnum::Fire:
				HealthParticles->SetTemplate(FireParticleSystem);
				HealthParticles->ActivateSystem();

				HealthSound->SetSound(FireSoundCue);
				HealthSound->Activate();
				break;
			case HealthStageEnum::Explosion:
				HealthParticles->SetTemplate(ExplosionParticleSystem);
				HealthParticles->ActivateSystem();

				HealthSound->SetSound(ExplosionSoundCue);
				HealthSound->Activate();

				GetGunPawn()->Destroy();
				DestroyCar();// TODO: Use timer to be able to see and hear the explosion
				break;
		}
	}
}

void ABase_DrivePawn::DestroyCar() {
	Destroy();
}

void ABase_DrivePawn::FlipCar() {
	if (WhileFlipping) {
		return;
	}

	if (FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) > 0.4) {
		return;
	}

	if (FMath::Abs(GetRootComponent()->GetComponentRotation().Roll) < 120) {
		return;
	}

	WhileFlipping = true;

	GetMesh()->SetSimulatePhysics(false);

	OldFlipCarRotation = GetTransform().Rotator();

	FlipCarTimeline->PlayFromStart();
}
void ABase_DrivePawn::FlipCarTimelineCallback(float val) {
	FRotator rotation = OldFlipCarRotation;

	rotation.Pitch = UKismetMathLibrary::MapRangeClamped(val, 0, 1, rotation.Pitch, 0);
	rotation.Roll = UKismetMathLibrary::MapRangeClamped(val, 0, 1, rotation.Roll, 0);

	SetActorRotation(rotation, ETeleportType::TeleportPhysics);
}
void ABase_DrivePawn::FlipCarTimelineFinishedCallback() {
	GetMesh()->SetSimulatePhysics(true);

	WhileFlipping = false;
}

void ABase_DrivePawn::SetSkeletonColor(FVector color) {
	SkeletonMeshMaterialInstance->SetVectorParameterValue("DiffuseColor", FLinearColor(color.X, color.Y, color.Z, 0));
	GunPawn->GetMaterial()->SetVectorParameterValue("DiffuseColor", FLinearColor(color.X, color.Y, color.Z, 0));
}

void ABase_DrivePawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
    DOREPLIFETIME(ABase_DrivePawn, Team);
}

void ABase_DrivePawn::SetSteering(float steering) {
	GetVehicleMovement()->SetSteeringInput(steering);
}
void ABase_DrivePawn::SetThrottle(float throttle) {
	GetVehicleMovement()->SetThrottleInput(throttle);
}

#undef LOCTEXT_NAMESPACE
