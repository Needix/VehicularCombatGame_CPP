// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_TimerItem.h"
#include "Engine/Texture2D.h"

void ABase_TimerItem::Use() {
	IsEnabled = true;
}

void ABase_TimerItem::Tick(float delta) {
	if(!IsEnabled) {
		return;
	}

	UseTick(delta);

	Timer -= delta;
	if(Timer <= 0) {
		Destroy();
	}
}

void ABase_TimerItem::UseTick(float delta) {
	
}


