// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_DrivePawn.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/AudioComponent.h"
#include "Components/TextRenderComponent.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED

#include "Player/Player_Controller.h"
#include "AllLevel/MyGameInstance.h"
#include "Base/Base_Pawn.h"

const FName APlayer_DrivePawn::LookUpBinding("LookUp");
const FName APlayer_DrivePawn::LookRightBinding("LookRight");

APlayer_DrivePawn::APlayer_DrivePawn() {
	// Super::Super();
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/VehicularCombatGame/Sound/Horn.Horn"));
	HornSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HornSound"));
	HornSoundComponent->bAutoActivate = false;
	HornSoundComponent->SetSound(SoundCue.Object);
	HornSoundComponent->SetupAttachment(GetMesh());
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
	
	PlayerInputComponent->BindAction("SwitchRole", IE_Released, this, &APlayer_DrivePawn::OnSwitchRole);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &APlayer_DrivePawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &APlayer_DrivePawn::OnHandbrakeReleased);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APlayer_DrivePawn::OnResetVR);
	PlayerInputComponent->BindAction("Horn", IE_Pressed, this, &APlayer_DrivePawn::OnHorn);

	PlayerInputComponent->BindAction("ConnectToServer", IE_Pressed, this, &APlayer_DrivePawn::OnJoinServer);
	PlayerInputComponent->BindAction("HostServer", IE_Pressed, this, &APlayer_DrivePawn::OnHostServer);
}

void APlayer_DrivePawn::OnSwitchRole() {
	Base_Pawn::SwitchRoles(this, GunPawn);
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

void APlayer_DrivePawn::OnHostServer() {
	APlayer_Controller *pc = CastChecked<APlayer_Controller>(this->GetController());
	UE_LOG(LogTemp, Warning, TEXT("OnHostServer for %i"), pc->NetPlayerIndex);
	UMyGameInstance *gameInstance = CastChecked<UMyGameInstance>(GetGameInstance());
	gameInstance->Host(pc->NetPlayerIndex);
}

void APlayer_DrivePawn::OnJoinServer() {
	APlayer_Controller *pc = CastChecked<APlayer_Controller>(this->GetController());
	UE_LOG(LogTemp, Warning, TEXT("OnJoinServer for %i"), pc->NetPlayerIndex);
	UMyGameInstance *gameInstance = CastChecked<UMyGameInstance>(GetGameInstance());
	gameInstance->Join(pc->NetPlayerIndex);
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

void APlayer_DrivePawn::OnHorn() {
	HornSoundComponent->Play();
}