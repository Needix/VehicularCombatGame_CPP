// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTF_Flag.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API ACTF_Flag : public AActor
{
	GENERATED_BODY()
	
//private:
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	ACTF_Flag();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void BoxCollisionComponentOverlap(class AActor* myActor, class AActor* otherActor);
	
};
