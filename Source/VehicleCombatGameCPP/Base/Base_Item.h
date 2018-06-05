// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"

#include "Particles/ParticleSystemComponent.h"
#include "Components/BillboardComponent.h"

#include "Base_Item.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API ABase_Item : public AActor
{
	GENERATED_BODY()
	
private:
	UMaterialInstanceDynamic* MaterialInstance;
	UParticleSystemComponent* ParticleSystem;
	UBillboardComponent* IconBillboard;
	UTexture2D* ItemIcon;

	class ABase_GunPawn* GunPawn;

	// Status
	bool PickedUp;

protected:
	// Ammo
	UPROPERTY(BlueprintReadOnly)
	int Ammo;
	int MaxAmmo;

	bool NeedsTarget = true;

	bool IsPrimaryFiring;

private:
	void SetupComponents();
	void SetupEvents();

	UFUNCTION()
	void BoxCollisionComponentOverlap(class AActor* myActor, AActor* otherActor);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetItemIcon(UTexture2D* texture) {
		ItemIcon = texture;
	}

public:	
	ABase_Item();

	virtual void OnPrimaryFirePressed();
	virtual void OnPrimaryFireReleased();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	bool IsNeedingTarget() {
		return NeedsTarget;
	}
	
	void SetGunPawn(class ABase_GunPawn* gunPawn) {
		GunPawn = gunPawn;
	}
	ABase_GunPawn* GetGunPawn() {
		return GunPawn;
	}
};
