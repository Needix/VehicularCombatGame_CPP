// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_Controller.h"

void APlayer_Controller::Tick(float Delta) {
	APawn *pawn = GetPawn();
	if (!IsValid(pawn)) {
		RespawnTimer += Delta;
	} else {
		RespawnTimer = 0;
	}
}

void APlayer_Controller::ChangeMenuWidget(TSubclassOf<UUserWidget> newWidgetClass) {
	if (CurrentWidget != nullptr) {
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	bShowMouseCursor = newWidgetClass != nullptr;

	if (newWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), newWidgetClass);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport(100);
		}
	}
}