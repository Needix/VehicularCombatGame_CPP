// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_DrivePawn.h"
#include "Player/Player_DrivePawn.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"

void AAI_DrivePawn::Tick(float Delta) {
	Super::Tick(Delta);

	UWheeledVehicleMovementComponent4W *Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	Vehicle4W->SetHandbrakeInput(false);
}
