// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_Controller.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#include "Base/Base_DrivePawn.h"
#include "Base/Base_GunPawn.h"
#include "GameModes/Team.h"

void AAI_Controller::Tick(float delta) {
	APawn* pawn = GetPawn();
	if(!IsValid(pawn)) {
		return;
	}

	if(pawn->GetClass()->IsChildOf(ABase_DrivePawn::StaticClass())) {
		ABase_DrivePawn* drivePawn = CastChecked<ABase_DrivePawn>(pawn);
		AActor* target = GetDriveTarget(drivePawn);

		ABase_Item* item = GetClosestItem(drivePawn);
		if(drivePawn->GetDistanceTo(item) < drivePawn->GetDistanceTo(target) / 10) {
			target = item;
		}

		//HandleDrivePawn(drivePawn, target);
	} else if(pawn->GetClass()->IsChildOf(ABase_GunPawn::StaticClass())) {
		ABase_GunPawn* gunPawn = CastChecked<ABase_GunPawn>(pawn);
		AActor* target = GetGunTarget(gunPawn);
		HandleGunPawn(gunPawn, target);
	}
}

void AAI_Controller::HandleDrivePawn(ABase_DrivePawn* pawn, AActor* target) {
	if(IsValid(pawn) && IsValid(target)) {
		SetThrottle(target);
		SetSteering(target);
	}
}
void AAI_Controller::HandleGunPawn(ABase_GunPawn* pawn, AActor* target) {
	if(IsValid(pawn->GetItem()) && ((IsValid(pawn) && IsValid(target)) || !pawn->GetItem()->IsNeedingTarget())) { // Check for valid item. If valid item, either fire if we have a target in range, or the item does not need an target (e.g. health)
		pawn->OnPrimaryFirePressed();
		FRotator baseRotator = GetBaseRotator(target->GetActorLocation());
		pawn->SetLookUpValue(baseRotator.Pitch + 0.11);
		pawn->SetLookRightValue(baseRotator.Yaw);
	} else {
		pawn->OnPrimaryFireReleased();
	}
}

AActor* AAI_Controller::GetDriveTarget(ABase_DrivePawn* pawn) {
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Append(pawn->GetTeam()->GetTeamPlayerAsDrivePawns());
	return FindClosestActorOfType(ABase_DrivePawn::StaticClass(), false, actorsToIgnore);
}

AActor* AAI_Controller::GetGunTarget(ABase_GunPawn* pawn) {
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Append(pawn->GetDrivePawn()->GetTeam()->GetTeamPlayerAsDrivePawns());
	return FindClosestActorOfType(ABase_DrivePawn::StaticClass(), true, actorsToIgnore);
}

ABase_Item* AAI_Controller::GetClosestItem(ABase_DrivePawn* pawn) {
	TArray<AActor*> actorsToIgnore;
	AActor* closestItem = FindClosestActorOfType(ABase_Item::StaticClass(), false, actorsToIgnore);
	if(IsValid(closestItem)) {
		return CastChecked<ABase_Item>(closestItem);
	}
	return NULL;
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
		FRotator baseRotator = GetBaseRotator(location);
		pawn->SetSteering(UKismetMathLibrary::MapRangeClamped(baseRotator.Yaw, -100, 100, -1, 1));
	}
}

FRotator AAI_Controller::GetBaseRotator(FVector location) {
	FRotator rotator = UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), location);
	FRotator myRotator = GetPawn()->GetActorRotation();
	return UKismetMathLibrary::NormalizedDeltaRotator(rotator, myRotator);
}


AActor *AAI_Controller::FindClosestActorOfType(UClass* actorClass, bool withLineOfSight, TArray<AActor*> actorsToIgnore) {
	AActor *closest = NULL;
	float nearestPlayerDistance = -1;

	TArray<AActor *> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), actorClass, actors);

	for (AActor* actor : actors) {
		if(!actor->GetRootComponent()->IsVisible()) {
			continue;
		}

		bool ignoreActorFound = false;
		for(AActor* actor2 : actorsToIgnore) {
			if(actor == actor2) {
				ignoreActorFound = true;
			}
		}
		if(ignoreActorFound) {
			continue;
		}

		if(withLineOfSight) {
			FHitResult outHit;
			TArray<AActor *> actorsToIgnoreForTrace;
			FVector sourceLocation = GetPawn()->GetActorLocation();
			sourceLocation.Z += 50;
			FVector targetLocation = actor->GetActorLocation();
			targetLocation.Z += 40;
			FVector source2TargetVector = targetLocation - sourceLocation;
			source2TargetVector.Normalize();
			FVector source = sourceLocation + source2TargetVector * 200;
			bool traceSuccess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), source, targetLocation, ETraceTypeQuery::TraceTypeQuery_MAX, false, actorsToIgnoreForTrace, EDrawDebugTrace::None, outHit, true, FLinearColor::Red, FLinearColor::Green, 1);
			if(!traceSuccess || outHit.Actor.Get() != actor) {
				continue;
			}
		}
		
		float distance = GetPawn()->GetDistanceTo(actor);
		if (nearestPlayerDistance == -1 || distance < nearestPlayerDistance) {
			nearestPlayerDistance = distance;
			closest = actor;
		}
	}
	return closest;
}
