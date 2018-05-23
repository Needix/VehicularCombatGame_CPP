// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_DrivePawn.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Components/TextRenderComponent.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED

const FName APlayer_DrivePawn::LookUpBinding("LookUp");
const FName APlayer_DrivePawn::LookRightBinding("LookRight");

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
	UpdateHMDCamera();
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
	EnableIncarView(!bInCarCameraActive);
}


