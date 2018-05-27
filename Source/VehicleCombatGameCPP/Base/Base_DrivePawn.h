// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Components/TimelineComponent.h"
#include "Base_DrivePawn.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
class UAudioComponent;

UCLASS(config = Game)
class ABase_DrivePawn : public AWheeledVehicle {
	GENERATED_BODY()

	// Car components
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *SpringArm;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *Camera;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent *InternalCameraBase;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *InternalCamera;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent *InCarSpeed;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent *InCarGear;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent *EngineSoundComponent;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent *ExhaustParticles;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent *HealthSmoke;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent *HealthFire;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent *HealthExplosion;

  public:
	ABase_DrivePawn();
	
	
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText SpeedDisplayString;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText GearDisplayString;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FColor GearDisplayColor;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FColor GearDisplayReverseColor;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInCarCameraActive;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInReverseGear;

	UPROPERTY()
	UTimelineComponent *FlipCarTimeline;

	UFUNCTION()
	void FlipCarTimelineCallback(float val);

	UFUNCTION()
	void FlipCarTimelineFinishedCallback();

	FVector InternalCameraOrigin;

	class ATeam* Team;

  private:
	// Update physics. Slippery = car on the roof (slide); NonSlippery = Normal driving
	bool bIsLowFriction;
	UPhysicalMaterial *SlipperyMaterial;
	UPhysicalMaterial *NonSlipperyMaterial;

	// Mesh Color
	UMaterialInstanceDynamic* SkeletonMeshMaterialInstance;

	// Health
	float Health = 100;
	bool IsCollidingWithKillPlane;

	float CurrentDeltaSeconds;

	FRotator OldFlipCarRotation;
	bool WhileFlipping;

  public:
	virtual void Tick(float Delta) override;

	void SetSkeletonColor(FVector color);

	// Interact with the car health
	void DecreaseHealthByTime(float deltaSecondsMultiplicator);
	void DecreaseHealthByFloat(float health);
	void DestroyCar();

	static const FName EngineAudioRPM;

  protected:
	virtual void BeginPlay() override;

  private:
	// Camera/HUD
	void EnableIncarView(const bool bState);

	/*UFUNCTION()
	void ActorBeginOverlap1(class AActor* actor, class AActor* otherActor);*/
	UFUNCTION()
	void HandleOverlapStartEvent(class AActor* myActor, class AActor* otherActor);
	UFUNCTION()
	void HandleOverlapEndEvent(class AActor* myActor, class AActor* otherActor);
	UFUNCTION()
	void HandleHitEvent(UPrimitiveComponent* hitComponent, AActor* actor, UPrimitiveComponent* otherComponent, FVector normalImpulse, FHitResult& hit);

	// Used to initialize the car at spawn. Called only once at "BeginPlay"
	void InitializeEventDelegates();
	void InitializeBasicComponents();
	void InitializeCar();
	void InitializeParticleSystems();
	void InitializeOtherComponents();
	void InitializeFlipCarTimeline();

	// These functions get called every frame and are used to update stuff
	void UpdateHealth();
	void UpdatePhysicsMaterial();
	void UpdateInCarHUD();
	void UpdateHUDStrings();
	void UpdateSound();
	void FlipCar();

  public:
    // Car Component Getter
	FORCEINLINE USpringArmComponent *GetSpringArm() const { return SpringArm; }
	FORCEINLINE UCameraComponent *GetCamera() const { return Camera; }
	FORCEINLINE UCameraComponent *GetInternalCamera() const { return InternalCamera; }
	FORCEINLINE UTextRenderComponent *GetInCarSpeed() const { return InCarSpeed; }
	FORCEINLINE UTextRenderComponent *GetInCarGear() const { return InCarGear; }
	FORCEINLINE UAudioComponent *GetEngineSoundComponent() const { return EngineSoundComponent; }

	UFUNCTION(BlueprintCallable)
	float GetHealth() { return Health; }
};
