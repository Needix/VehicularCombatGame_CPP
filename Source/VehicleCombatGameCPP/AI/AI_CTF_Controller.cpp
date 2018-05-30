// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_CTF_Controller.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameModes/Team.h"

#include "GameModes/CTF/CTF_Flag.h"

AActor* AAI_CTF_Controller::GetDriveTarget(ABase_DrivePawn* pawn) {
	ACTF_Flag* flag = FindFlag();
	if(flag == NULL) {
		return Super::GetDriveTarget(pawn);
	}
	
	if(flag->GetDrivePawn() == pawn) {
		return pawn->GetTeam();
	} else {
		return flag;
	}
}
AActor* AAI_CTF_Controller::GetGunTarget(ABase_GunPawn* pawn) {
	return Super::GetGunTarget(pawn);
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
