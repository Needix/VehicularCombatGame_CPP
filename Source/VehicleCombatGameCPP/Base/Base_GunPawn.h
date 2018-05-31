// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Base/Base_Item.h"

#include "Base_GunPawn.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API ABase_GunPawn : public APawn {
	GENERATED_BODY()

  private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* base;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* YawBase;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* sceneComponent1;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* cube1;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* cube2;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* PitchBase;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* barrelBase;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* cameraComponent;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Barrel;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* barrelInsde;
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* SkeletonMeshMaterialInstance;

	static const FName LookUpBinding;
	static const FName LookRightBinding;

	bool IsPrimaryFiring;

	UPROPERTY()
	ABase_Item* Item;

	class ABase_DrivePawn* DrivePawn;
	
	float LookUpValue = 0;
	float LookRightValue = 0;

  private:
	void SetupComponents();

  protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  public:
	ABase_GunPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void HandleMovement(float deltaTime);
	void HandlePrimaryFire(float deltaTime);


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	void OnPrimaryFirePressed() {
		IsPrimaryFiring = true;
	}
	void OnPrimaryFireReleased() {
		IsPrimaryFiring = false;
	}
	void OnSwitchRole();


	void SetColor(FVector color);

	void SetItem(ABase_Item* item) {
		Item = item;
	}

	UFUNCTION(BlueprintCallable)
	ABase_DrivePawn* GetDrivePawn() {
		return DrivePawn;
	}
	void SetDrivePawn(ABase_DrivePawn* pawn) {
		DrivePawn = pawn;
	}

	UMaterialInstanceDynamic* GetMaterial() {
		return SkeletonMeshMaterialInstance;
	}

	void SetLookUpValue(float value) {
		LookUpValue = value;
	}

	void SetLookRightValue(float value) {
		LookRightValue = value;
	}
};
