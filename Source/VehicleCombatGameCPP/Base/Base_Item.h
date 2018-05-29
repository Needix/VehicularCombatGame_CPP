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

	// Status
	bool PickedUp;

protected:
	// Ammo
	int Ammo;
	int MaxAmmo;

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

	virtual void Use();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
