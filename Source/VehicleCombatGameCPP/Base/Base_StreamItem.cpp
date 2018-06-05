// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_StreamItem.h"
#include "Engine/Texture2D.h"


void ABase_StreamItem::Tick(float delta) {
	AmmoPerMinuteTimer += delta;
	if(AmmoPerMinuteTimer > (60 / AmmoPerMinute)) {
		if(IsPrimaryFiring) {
			FirePrimary();
			AmmoPerMinuteTimer = 0;
		}
	}
}

void ABase_StreamItem::FirePrimary() {
	Ammo -= 1;
	if(Ammo <= 0) {
		Destroy();
	}
}

