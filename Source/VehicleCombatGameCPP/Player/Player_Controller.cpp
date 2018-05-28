// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_Controller.h"
#include "UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "AllLevel/Singleton.h"
#include "UI/Widget_Gameplay.h"
#include "Widget.h"

void APlayer_Controller::BeginPlay() {
	if(this->IsLocalController()) {
		TArray<AActor *> singletons;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASingleton::StaticClass(), singletons);
		check(singletons.Num() == 1);
		ASingleton* singleton = CastChecked<ASingleton>(singletons[0]);

		Widget_Gameplay = CreateWidget<UWidget_Gameplay>(this, singleton->wGameplayWidget);
		if(IsValid(Widget_Gameplay)) {
			Widget_Gameplay->SetOwningPlayer(this);
			Widget_Gameplay->AddToViewport(5000);
		}
	}
}

void APlayer_Controller::Tick(float Delta) {
	APawn* pawn = GetPawn();
	if(!IsValid(pawn)) {
		RespawnTimer += Delta;
	} else {
		RespawnTimer = 0;
	}
}

