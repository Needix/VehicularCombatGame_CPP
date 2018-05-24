// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_DrivePawn.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED

const FName APlayer_DrivePawn::LookUpBinding("LookUp");
const FName APlayer_DrivePawn::LookRightBinding("LookRight");

APlayer_DrivePawn::APlayer_DrivePawn() {
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/VehicularCombatGame/Sound/Horn.Horn"));
	HornSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HornSound"));
	HornSoundComponent->bAutoActivate = false;
	HornSoundComponent->SetSound(SoundCue.Object);
	HornSoundComponent->SetupAttachment(GetMesh());

	Cameras.Add(FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1, 1, 1)));
	Cameras.Add(FTransform(FRotator(0, 10, 0), FVector(-193.94, 0, -12.16), FVector(1, 1, 1)));
	Cameras.Add(FTransform(FRotator(0, 0, 0), FVector(-65.9, 0, 10.42), FVector(1, 1, 1)));

	SetupCameraTransitionTimeline();
}

void APlayer_DrivePawn::BeginPlay() {
	Super::BeginPlay();
	bool bWantInCar = false;
	// Enable in car view if HMD is attached
#if HMD_MODULE_INCLUDED
	bWantInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
	EnableIncarView(bWantInCar);
}

void APlayer_DrivePawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsValid(CameraTransitionTimeline)) {
		CameraTransitionTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}

	UpdateHMDCamera();
}

void APlayer_DrivePawn::SetupCameraTransitionTimeline() {
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/VehicularCombatGame/Curves/CameraTransition.CameraTransition"));

	CameraTransitionTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
	CameraTransitionTimeline->SetPropertySetObject(this);

	FOnTimelineFloat callback;
	FOnTimelineEventStatic onFinishedCallback;

	callback.BindUFunction(this, FName{TEXT("CameraTransitionTimelineCallback")});
	CameraTransitionTimeline->AddInterpFloat(Curve.Object, callback);

	onFinishedCallback.BindUFunction(this, FName{TEXT("CameraTransitionTimelineFinishedCallback")});
	CameraTransitionTimeline->SetTimelineFinishedFunc(onFinishedCallback);

	CameraTransitionTimeline->RegisterComponent();
}

void APlayer_DrivePawn::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayer_DrivePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayer_DrivePawn::MoveRight);
	PlayerInputComponent->BindAxis(LookUpBinding);
	PlayerInputComponent->BindAxis(LookRightBinding);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &APlayer_DrivePawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &APlayer_DrivePawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &APlayer_DrivePawn::OnToggleCamera);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APlayer_DrivePawn::OnResetVR);
	PlayerInputComponent->BindAction("Horn", IE_Pressed, this, &APlayer_DrivePawn::OnHorn);
}

void APlayer_DrivePawn::MoveForward(float Val) {
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void APlayer_DrivePawn::MoveRight(float Val) {
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void APlayer_DrivePawn::OnHandbrakePressed() {
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void APlayer_DrivePawn::OnHandbrakeReleased() {
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void APlayer_DrivePawn::UpdateHMDCamera() {
	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->XRSystem.IsValid() == true) && ((GEngine->XRSystem->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true))) {
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (!bHMDActive) {
		if (InputComponent && bInCarCameraActive) {
			FRotator HeadRotation = GetInternalCamera()->RelativeRotation;
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			GetInternalCamera()->RelativeRotation = HeadRotation;
		}
	}
}

void APlayer_DrivePawn::EnableIncarView(const bool bState) {
	if (bState != bInCarCameraActive) {
		bInCarCameraActive = bState;

		if (bState) {
			OnResetVR();
			GetCamera()->Deactivate();
			GetInternalCamera()->Activate();
		} else {
			GetInternalCamera()->Deactivate();
			GetCamera()->Activate();
		}

		GetInCarSpeed()->SetVisibility(bInCarCameraActive);
		GetInCarGear()->SetVisibility(bInCarCameraActive);
	}
}

void APlayer_DrivePawn::OnResetVR() {
#if HMD_MODULE_INCLUDED
	if (GEngine->XRSystem.IsValid()) {
		GEngine->XRSystem->ResetOrientationAndPosition();
		GetInternalCamera()->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void APlayer_DrivePawn::OnToggleCamera() {
	int newIndex = CurrentCamera + 1;

	if (newIndex >= Cameras.Num()) {
		newIndex = 0;
	}

	CurrentCamera = newIndex;

	OldCameraTransform = GetCamera()->GetRelativeTransform();

	if (IsValid(CameraTransitionTimeline)) {
		CameraTransitionTimeline->PlayFromStart();
	}
}

void APlayer_DrivePawn::OnHorn() {
	HornSoundComponent->Play();
}

void APlayer_DrivePawn::CameraTransitionTimelineCallback(float val) {
	FTransform transform = UKismetMathLibrary::TLerp(OldCameraTransform, Cameras[CurrentCamera], val);

	GetCamera()->SetRelativeLocationAndRotation(transform.GetLocation(), transform.GetRotation());
}

void APlayer_DrivePawn::CameraTransitionTimelineFinishedCallback() {
	EnableIncarView(CurrentCamera == 0);
}