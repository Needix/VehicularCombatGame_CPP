// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthRegen.h"

#include "ConstructorHelpers.h"

#include "Base/Base_DrivePawn.h"
#include "Base/Base_GunPawn.h"

AHealthRegen::AHealthRegen() {
	Timer = 20;
	
	ConstructorHelpers::FObjectFinder<UTexture2D> sprite(TEXT("Texture2D'/Game/VehicularCombatGame/Icons/SpriteRepair_128x128.SpriteRepair_128x128'"));
	SetItemIcon(sprite.Object);
}

void AHealthRegen::UseTick(float delta) {
	ABase_DrivePawn* owner = CastChecked<ABase_GunPawn>(GetOwner())->GetDrivePawn();
	owner->IncreaseHealthByFloat(RegenAmountPerSecond * delta);
	Super::UseTick(delta);
}

