// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_Pawn.h"

#include "Engine/Engine.h"
#include "GameFramework/Controller.h"

#include "Base/Base_DrivePawn.h"
#include "Base/Base_GunPawn.h"
#include "GameModes/Team.h"

Base_Pawn::Base_Pawn() {
}

Base_Pawn::~Base_Pawn() {
}


void Base_Pawn::SwitchRoles(ABase_DrivePawn* drivePawn, ABase_GunPawn* gunPawn) {
	AController* driveController = drivePawn->GetController();
	AController* gunController = gunPawn->GetController();
	
	for(int i = 0; i < drivePawn->GetTeam()->TeamPlayer.Num(); i++) {
		AController* controller = drivePawn->GetTeam()->TeamPlayer[i];
		if(controller == driveController) {
			drivePawn->GetTeam()->TeamPlayer[i] = gunController;
			break;
		}
	}

	driveController->UnPossess();
	gunController->UnPossess();
	driveController->Possess(gunPawn);
	gunController->Possess(drivePawn);
}