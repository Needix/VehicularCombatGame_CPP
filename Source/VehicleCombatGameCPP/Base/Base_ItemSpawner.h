// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Base/Base_Item.h"

#include "Base_ItemSpawner.generated.h"

class ABase_GameMode;

UCLASS()
class VEHICLECOMBATGAMECPP_API ABase_ItemSpawner : public AActor
{
	GENERATED_BODY()
	
private:
	TArray<TSubclassOf<ABase_Item>> ItemTypes;
	TArray<ABase_Item*> GroundItems;
	ABase_GameMode* GameMode;
	float RespawnTimer;

protected:
	const float ItemRespawnTime = 3;
	const int MaxItems = 100;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	ABase_ItemSpawner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ABase_Item* SpawnRandomItem();
	ABase_Item* SpawnItem(TSubclassOf<ABase_Item> itemClassToSpawn);
	TSubclassOf<ABase_Item> GetRandomItemClass();
	
};
