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

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent *HornSoundComponent;

  public:
	APlayer_DrivePawn();
	// INPUTS
	void MoveForward(float Val);
	void MoveRight(float Val);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();
	void OnToggleCamera();
	void OnResetVR();
	void OnHorn();

	virtual void SetupPlayerInputComponent(UInputComponent *InputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void UpdateHMDCamera();

	static const FName LookUpBinding;
	static const FName LookRightBinding;

  private:
	void EnableIncarView(const bool bState);
};
