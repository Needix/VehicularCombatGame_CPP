// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "AllLevel/Singleton.h"
#include "GameModes/Team.h"
#include "Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API APlayer_Controller : public APlayerController {
	GENERATED_BODY()
		
	public:
		ATeam* Team;
		float RespawnTimer = 50000;
		
		UFUNCTION(BlueprintCallable, Category = "UMG Game")
		void ChangeMenuWidget(TSubclassOf<UUserWidget> newWidgetClass);

		UFUNCTION()
		void CameraTransitionTimelineCallback(float val);

		UFUNCTION()
		void CameraTransitionTimelineFinishedCallback();

		protected:
		UPROPERTY()
		UUserWidget *CurrentWidget;

		UPROPERTY()
		TArray<FTransform> Cameras;

		UPROPERTY()
		UTimelineComponent *CameraTransitionTimeline;

  private:
		int CurrentCamera = 0;
		FTransform OldCameraTransform;
		ASingleton *Singleton;

  private:
		void SetupCameraTransitionTimeline();

  public:
		virtual void BeginPlay() override;
		virtual void Tick(float Delta) override;
		virtual void SetupInputComponent() override;

  public:
		APlayer_Controller();

		void OnToggleCamera();
		void OnPauseMenu();
};