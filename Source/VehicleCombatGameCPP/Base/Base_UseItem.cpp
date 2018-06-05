// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_UseItem.h"
#include "Engine/Texture2D.h"


void ABase_UseItem::Tick(float delta) {
	if(IsPrimaryFiring) {
		if(!WasUsed) {
			ABase_UseItem::Use();
			WasUsed = true;
		}
	} else {
		WasUsed = false;
	}
}

void ABase_UseItem::Use() {
	Ammo -= 1;
}


