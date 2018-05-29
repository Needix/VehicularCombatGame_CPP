// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Components/TimelineComponent.h"
#include "Sound/SoundCue.h"
#include "Base_DrivePawn.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
class UAudioComponent;
class ABase_GunPawn;

enum class HealthStageEnum : uint8 { Normal, Smoke, Fire, Explosion };

UCLASS(config = Game)
class ABase_DrivePawn : public AWheeledVehicle {
	GENERATED_BODY()

	// Car components
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *SpringArm;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *Camera;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *TopDownCamera;

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
	class UParticleSystemComponent *HealthParticles;

	UPROPERTY(VisibleAnywhere)
	class UAudioComponent *HealthSound;

  public:
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

  private:
	ABase_GunPawn* GunPawn;

	// Update physics. Slippery = car on the roof (slide); NonSlippery = Normal driving
	bool bIsLowFriction;
	UPhysicalMaterial *SlipperyMaterial;
	UPhysicalMaterial *NonSlipperyMaterial;
	UParticleSystem *SmokeParticleSystem;
	UParticleSystem *FireParticleSystem;
	UParticleSystem *ExplosionParticleSystem;
	USoundCue *SmokeSoundCue;
	USoundCue *FireSoundCue;
	USoundCue *ExplosionSoundCue;

	// Mesh Color
	UMaterialInstanceDynamic *SkeletonMeshMaterialInstance;

	// Health
	float Health = 100;
	HealthStageEnum HealthStage = HealthStageEnum::Normal;
	bool IsCollidingWithKillPlane;

	float CurrentDeltaSeconds;

	UPROPERTY(Replicated)
	class ATeam *Team;

	// Flip car
	FRotator OldFlipCarRotation;
	bool WhileFlipping;

  public:
	ABase_DrivePawn();

	virtual void Tick(float Delta) override;

	void SetSkeletonColor(FVector color);

	// Interact with the car health
	void IncreaseHealthByFloat(float amount);
	void DecreaseHealthByTime(float deltaSecondsMultiplicator);
	void DecreaseHealthByFloat(float health);
	void DestroyCar();

	static const FName EngineAudioRPM;

  protected:
	virtual void BeginPlay() override;

  private:

	/*UFUNCTION()
	void ActorBeginOverlap1(class AActor* actor, class AActor* otherActor);*/
	UFUNCTION()
	void HandleOverlapStartEvent(class AActor *myActor, class AActor *otherActor);
	UFUNCTION()
	void HandleOverlapEndEvent(class AActor *myActor, class AActor *otherActor);
	UFUNCTION()
	void HandleHitEvent(UPrimitiveComponent *hitComponent, AActor *actor, UPrimitiveComponent *otherComponent, FVector normalImpulse, FHitResult &hit);

	// Used to initialize the car at spawn. Called only once at "BeginPlay"
	void InitializeEventDelegates();
	void InitializeBasicComponents();
	void InitializeCar();
	void InitializeParticleSystems();
	void InitializeSoundCues();
	void InitializeOtherComponents();
	void InitializeFlipCarTimeline();
	void InitializeTurretSpawn();

	// These functions get called every frame and are used to update stuff
	void UpdateHealth();
	void UpdatePhysicsMaterial();
	void UpdateInCarHUD();
	void UpdateHUDStrings();
	void UpdateSound();
	void UpdateCarColor();
	void FlipCar();

  public:
	// Car Component Getter
	FORCEINLINE USpringArmComponent *GetSpringArm() const {
		return SpringArm;
	}
	FORCEINLINE UCameraComponent *GetCamera() const {
		return Camera;
	}
	FORCEINLINE UCameraComponent *GetInternalCamera() const {
		return InternalCamera;
	}
	FORCEINLINE UTextRenderComponent *GetInCarSpeed() const {
		return InCarSpeed;
	}
	FORCEINLINE UTextRenderComponent *GetInCarGear() const {
		return InCarGear;
	}
	FORCEINLINE UAudioComponent *GetEngineSoundComponent() const {
		return EngineSoundComponent;
	}

	ATeam* GetTeam() {
		return Team;
	}
	void SetTeam(ATeam* team) {
		Team = team;
	}

	ABase_GunPawn* GetGunPawn() {
		return GunPawn;
	}

	UFUNCTION(BlueprintCallable)
	float GetHealth() {
		return Health;
	}
	
	void SetSteering(float steering);
	void SetThrottle(float throttle);
};
