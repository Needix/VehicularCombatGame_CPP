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
	// INPUTS
	void MoveForward(float Val);
	void MoveRight(float Val);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();
	void OnToggleCamera();
	void OnResetVR();

	virtual void SetupPlayerInputComponent(UInputComponent *InputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void UpdateHMDCamera();

	static const FName LookUpBinding;
	static const FName LookRightBinding;

  private:
	void EnableIncarView(const bool bState);
};
