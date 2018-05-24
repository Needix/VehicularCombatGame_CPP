// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_DrivePawn.h"
#include "Player/Player_DrivePawn.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"

void AAI_DrivePawn::Tick(float Delta) {
	Super::Tick(Delta);

	APlayer_DrivePawn *closestPlayer = FindClosestPlayer();
	UWheeledVehicleMovementComponent4W *Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	Vehicle4W->SetHandbrakeInput(false);

	if(closestPlayer != NULL && IsValid(closestPlayer)) {
		SetSteering(closestPlayer);
		SetThrottle(closestPlayer);
	}
}

void AAI_DrivePawn::SetThrottle(AActor *closestPlayer) {
	float distance = closestPlayer->GetDistanceTo(this);
	float throttle = UKismetMathLibrary::MapRangeClamped(distance, 0, 5000, 0.7, 1);

	UWheeledVehicleMovementComponent4W *Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	Vehicle4W->SetThrottleInput(throttle);
}

void AAI_DrivePawn::SetSteering(AActor *player) {
	FVector closestPlayerLocation = player->GetActorLocation();
	SetSteering(closestPlayerLocation);
}

void AAI_DrivePawn::SetSteering(FVector location) {
	FRotator rotator = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), location);
	FRotator myRotator = this->GetActorRotation();
	FRotator baseRotator = UKismetMathLibrary::NormalizedDeltaRotator(rotator, myRotator);
	float yaw = baseRotator.Yaw;
	float steering = UKismetMathLibrary::MapRangeClamped(yaw, -100, 100, -1, 1);

	UWheeledVehicleMovementComponent4W *Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	Vehicle4W->SetSteeringInput(steering);
}


APlayer_DrivePawn *AAI_DrivePawn::FindClosestPlayer() {
	APlayer_DrivePawn *closest = NULL;
	float nearestPlayerDistance = -1;
	for (TActorIterator<APlayer_DrivePawn> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		APlayer_DrivePawn *Mesh = *ActorItr;
		float distance = GetDistanceTo(Mesh);
		if (nearestPlayerDistance == -1 || distance < nearestPlayerDistance) {
			nearestPlayerDistance = distance;
			closest = Mesh;
		}
	}
	return closest;
}
