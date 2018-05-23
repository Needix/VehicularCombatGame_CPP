// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_Controller.h"


void APlayer_Controller::Tick(float Delta) {
	APawn* pawn = GetPawn();
	if(!IsValid(pawn)) {
		RespawnTimer += Delta;
	} else {
		RespawnTimer = 0;
	}
}

