// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_Controller.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#include "Base/Base_DrivePawn.h"
#include "Base/Base_GunPawn.h"

void AAI_Controller::Tick(float delta) {
	APawn* pawn = GetPawn();
	if(!IsValid(pawn)) {
		return;
	}

	if(pawn->GetClass()->IsChildOf(ABase_DrivePawn::StaticClass())) {
		HandleDrivePawn(CastChecked<ABase_DrivePawn>(pawn));
	} else if(pawn->GetClass()->IsChildOf(ABase_GunPawn::StaticClass())) {
		HandleGunPawn(CastChecked<ABase_GunPawn>(pawn));
	}
}

void AAI_Controller::HandleDrivePawn(ABase_DrivePawn* pawn) {
	AActor* actor = FindClosestActorOfType(ABase_DrivePawn::StaticClass());
	if(IsValid(actor)) {
		SetThrottle(actor);
		SetSteering(actor);
	}
}
void AAI_Controller::HandleGunPawn(ABase_GunPawn* pawn) {
	AActor* actor = FindClosestActorOfType(ABase_DrivePawn::StaticClass());
	if(IsValid(actor)) {
		// TODO:
	}
}

void AAI_Controller::SetThrottle(AActor *closestPlayer) {
	if(GetPawn()->GetClass()->IsChildOf(ABase_DrivePawn::StaticClass())) {
		ABase_DrivePawn* pawn = CastChecked<ABase_DrivePawn>(GetPawn());
		float distance = closestPlayer->GetDistanceTo(pawn);
		float throttle = UKismetMathLibrary::MapRangeClamped(distance, 0, 5000, 0.7, 1);
		pawn->SetThrottle(throttle);
	}
}

void AAI_Controller::SetSteering(AActor *player) {
	FVector closestPlayerLocation = player->GetActorLocation();
	SetSteering(closestPlayerLocation);
}

void AAI_Controller::SetSteering(FVector location) {
	if(GetPawn()->GetClass()->IsChildOf(ABase_DrivePawn::StaticClass())) {
		ABase_DrivePawn* pawn = CastChecked<ABase_DrivePawn>(GetPawn());

		FRotator rotator = UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), location);
		FRotator myRotator = GetPawn()->GetActorRotation();
		FRotator baseRotator = UKismetMathLibrary::NormalizedDeltaRotator(rotator, myRotator);
		float yaw = baseRotator.Yaw;
		float steering = UKismetMathLibrary::MapRangeClamped(yaw, -100, 100, -1, 1);
		pawn->SetSteering(steering);
	}
}


AActor *AAI_Controller::FindClosestActorOfType(UClass* actorClass) {
	AActor *closest = NULL;
	float nearestPlayerDistance = -1;

	TArray<AActor *> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), actorClass, actors);

	for (AActor* actor : actors) {
		float distance = GetPawn()->GetDistanceTo(actor);
		if (nearestPlayerDistance == -1 || distance < nearestPlayerDistance) {
			nearestPlayerDistance = distance;
			closest = actor;
		}
	}
	return closest;
}
