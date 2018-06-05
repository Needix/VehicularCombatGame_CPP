// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "MinigunBullet.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API AMinigunBullet : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(Category = Components, EditAnywhere)
	UStaticMeshComponent* Test;
	UPROPERTY(Category = Components, EditAnywhere)
	UStaticMeshComponent* Bullet;
	UPROPERTY(Category = Components, EditAnywhere)
	UBoxComponent* boxCollision;
	UPROPERTY(Category = Components, EditAnywhere)
	UParticleSystemComponent* particleSystemComponent;

	float SecondsBeforeDespawnTimer = 0;
	float SecondsBeforeDespawnTime = 5;
	float InitialSpeed = 2000;
	float Damage = 1;
	bool IsFlying = false;

private:
	void SetupComponents();

	void HandleHit(class AActor* otherActor);
	UFUNCTION()
	void HandleOverlapStartEvent(class AActor *myActor, class AActor *otherActor);
	UFUNCTION()
	void OnComponentHitFunction(UPrimitiveComponent *hitComponent, AActor *otherActor, UPrimitiveComponent *otherComponent, FVector normalImpulse, FHitResult &hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	AMinigunBullet();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
