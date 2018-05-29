// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_CTF_Controller.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameModes/Team.h"

#include "GameModes/CTF/CTF_Flag.h"

void AAI_CTF_Controller::HandleDrivePawn(ABase_DrivePawn* pawn) {
	ACTF_Flag* flag = FindFlag();
	if(flag == NULL) {
		return;
	}
	
	SetSteering(pawn, flag);
	SetThrottle(pawn, flag);

}
void AAI_CTF_Controller::HandleGunPawn(ABase_GunPawn* pawn) {
	
}


void AAI_CTF_Controller::SetThrottle(ABase_DrivePawn* myPawn, ACTF_Flag* flag) {
	if(flag->GetOwner() == myPawn) {
		Super::SetThrottle(myPawn->GetTeam());
	} else {
		Super::SetThrottle(flag);
	}
}

void AAI_CTF_Controller::SetSteering(ABase_DrivePawn* myPawn, ACTF_Flag* flag) {
	if(flag->GetOwner() == myPawn) {
		Super::SetSteering(myPawn->GetTeam());
	} else {
		Super::SetSteering(flag);
	}
}

ACTF_Flag* AAI_CTF_Controller::FindFlag() {
	TArray<AActor *> flags;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTF_Flag::StaticClass(), flags);
	if(flags.Num() != 0) {
		return CastChecked<ACTF_Flag>(flags[0]);
	} else {
		return NULL;
	}
}
