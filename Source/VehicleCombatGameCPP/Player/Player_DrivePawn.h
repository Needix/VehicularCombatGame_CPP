// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
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

	UPROPERTY()
	TArray<FTransform> Cameras;

	UPROPERTY()
	UTimelineComponent *CameraTransitionTimeline;

	UFUNCTION()
	void CameraTransitionTimelineCallback(float val);

	UFUNCTION()
	void CameraTransitionTimelineFinishedCallback();

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
	void OnHostServer();
	void OnJoinServer();

	virtual void SetupPlayerInputComponent(UInputComponent *InputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetupCameraTransitionTimeline();
	void UpdateHMDCamera();

	static const FName LookUpBinding;
	static const FName LookRightBinding;

  private:
	void EnableIncarView(const bool bState);
	int CurrentCamera = 0;
	FTransform OldCameraTransform;
};
