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
		ABase_DrivePawn* drivePawn = CastChecked<ABase_DrivePawn>(pawn);
		AActor* target = GetDriveTarget(drivePawn);

		ABase_Item* item = GetClosestItem(drivePawn);
		if(drivePawn->GetDistanceTo(item) < drivePawn->GetDistanceTo(target) / 10) {
			target = item;
		}

		HandleDrivePawn(drivePawn, target);
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
	if(IsValid(pawn) && IsValid(target)) {
		pawn->OnPrimaryFirePressed();
		FRotator baseRotator = GetBaseRotator(target->GetActorLocation());
		pawn->SetLookUpValue(UKismetMathLibrary::MapRangeClamped(baseRotator.Pitch, -50, 50, -1, 1));
		pawn->SetLookRightValue(UKismetMathLibrary::MapRangeClamped(baseRotator.Yaw, -50, 50, -1, 1));
	}
}

AActor* AAI_Controller::GetDriveTarget(ABase_DrivePawn* pawn) {
	return FindClosestActorOfType(ABase_DrivePawn::StaticClass(), false);
}

AActor* AAI_Controller::GetGunTarget(ABase_GunPawn* pawn) {
	return FindClosestActorOfType(ABase_DrivePawn::StaticClass(), true);
}

ABase_Item* AAI_Controller::GetClosestItem(ABase_DrivePawn* pawn) {
	AActor* closestItem = FindClosestActorOfType(ABase_Item::StaticClass(), false);
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


AActor *AAI_Controller::FindClosestActorOfType(UClass* actorClass, bool withLineOfSight) {
	AActor *closest = NULL;
	float nearestPlayerDistance = -1;

	TArray<AActor *> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), actorClass, actors);

	for (AActor* actor : actors) {
		if(!actor->GetRootComponent()->IsVisible()) {
			continue;
		}
		float distance = GetPawn()->GetDistanceTo(actor);

		if(withLineOfSight) {
			FHitResult outHit;
			TArray<AActor *> actorsToIgnore;
			TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
			bool traceSuccess = UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetPawn()->GetActorLocation(), actor->GetActorLocation(), ETraceTypeQuery::TraceTypeQuery_MAX, false, actorsToIgnore, EDrawDebugTrace::None, outHit, true);
			if(!traceSuccess || outHit.Actor != actor) {
				continue;
			}
		}

		if (nearestPlayerDistance == -1 || distance < nearestPlayerDistance) {
			nearestPlayerDistance = distance;
			closest = actor;
		}
	}
	return closest;
}
