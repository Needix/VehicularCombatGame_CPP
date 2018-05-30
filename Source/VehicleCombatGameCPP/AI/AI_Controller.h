// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Base/Base_DrivePawn.h"
#include "Base/Base_GunPawn.h"
#include "AIController.h"

#include "AI_Controller.generated.h"

/**
 *
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AAI_Controller : public AAIController {
	GENERATED_BODY()

	private:
		void HandleDrivePawn(ABase_DrivePawn* pawn, AActor* target);
		void HandleGunPawn(ABase_GunPawn* pawn, AActor* target);
		ABase_Item* GetClosestItem(ABase_DrivePawn* pawn);

	protected:
		virtual AActor* GetDriveTarget(ABase_DrivePawn* pawn);
		virtual AActor* GetGunTarget(ABase_GunPawn* pawn);

	public:
		void Tick(float delta) override;
		class AActor *FindClosestActorOfType(UClass* actorClass, bool withLineOfSight);
		FRotator GetBaseRotator(FVector location);
		void SetSteering(AActor* actor);
		void SetSteering(FVector location);
		void SetThrottle(AActor *actor);
};
