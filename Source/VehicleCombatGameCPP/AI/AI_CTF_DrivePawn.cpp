// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_CTF_DrivePawn.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#include "GameModes/Team.h"
#include "GameModes/CTF/CTF_Flag.h"

void AAI_CTF_DrivePawn::Tick(float delta) {
	ACTF_Flag* flag = FindFlag();
	if(flag == NULL) {
		return;
	}
	
	SetSteering(flag);
	SetThrottle(flag);
}

void AAI_CTF_DrivePawn::SetThrottle(AActor* actor) {
	ACTF_Flag* flag = CastChecked<ACTF_Flag>(actor);
	if(flag->GetOwner() == this) {
		Super::SetThrottle(Team);
	} else {
		Super::SetThrottle(actor);
	}
}

void AAI_CTF_DrivePawn::SetSteering(AActor* actor) {
	ACTF_Flag* flag = CastChecked<ACTF_Flag>(actor);
	if(flag->GetOwner() == this) {
		Super::SetSteering(Team);
	} else {
		Super::SetSteering(actor);
	}
}

ACTF_Flag* AAI_CTF_DrivePawn::FindFlag() {
	TArray<AActor *> flags;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTF_Flag::StaticClass(), flags);
	if(flags.Num() != 0) {
		return CastChecked<ACTF_Flag>(flags[0]);
	} else {
		return NULL;
	}
}
