// Fill out your copyright notice in the Description page of Project Settings.

#include "Health.h"

#include "ConstructorHelpers.h"

#include "Base/Base_GunPawn.h"
#include "Base/Base_DrivePawn.h"

AHealth::AHealth() {
	ConstructorHelpers::FObjectFinder<UTexture2D> sprite(TEXT("Texture2D'/Game/VehicularCombatGame/Icons/SpriteRepair_128x128.SpriteRepair_128x128'"));
	SetItemIcon(sprite.Object);
}

void AHealth::Use() {
	ABase_DrivePawn* owner = CastChecked<ABase_GunPawn>(GetOwner())->GetDrivePawn();
	owner->IncreaseHealthByFloat(HealAmount);
	Super::Use();
}


