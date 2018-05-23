// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Base_DrivePawn.h"
#include "Player_DrivePawn.generated.h"

/**
 *
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API APlayer_DrivePawn : public ABase_DrivePawn {
	GENERATED_BODY()

  public:
	// INPUT
	/** Handle pressing forwards */
	void MoveForward(float Val);
	/** Handle pressing right */
	void MoveRight(float Val);
	/** Handle handbrake pressed */
	void OnHandbrakePressed();
	/** Handle handbrake released */
	void OnHandbrakeReleased();
	/** Switch between cameras */
	void OnToggleCamera();
	/** Handle reset VR device */
	void OnResetVR();

	virtual void SetupPlayerInputComponent(UInputComponent *InputComponent) override;
	virtual void BeginPlay() override;

  private:
	void EnableIncarView(const bool bState);
};
