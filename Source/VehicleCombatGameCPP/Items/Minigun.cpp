// Fill out your copyright notice in the Description page of Project Settings.

#include "Minigun.h"

#include "ConstructorHelpers.h"
#include "Engine/Engine.h"

#include "Base/Base_GunPawn.h"
#include "Items/SubItems/Minigun/MinigunBullet.h"

AMinigun::AMinigun() {
	Ammo = 5000;
	AmmoPerMinute = 2500;
	
	ConstructorHelpers::FObjectFinder<UTexture2D> sprite(TEXT("Texture2D'/Game/VehicularCombatGame/Icons/SpriteMachineGun_128x128.SpriteMachineGun_128x128'"));
	SetItemIcon(sprite.Object);	
}

void AMinigun::FirePrimary() {
	if(!IsValid(this) || !IsValid(GetGunPawn())) {
		return;
	}

	FActorSpawnParameters spawnParameters = FActorSpawnParameters();
	spawnParameters.Owner = this;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform transform = GetGunPawn()->GetGunBarrelEndTransform();
	AMinigunBullet* bullet = GetWorld()->SpawnActor<AMinigunBullet>(AMinigunBullet::StaticClass(), transform, spawnParameters);
	Super::FirePrimary();
}
