// Fill out your copyright notice in the Description page of Project Settings.

#include "Base_ItemSpawner.h"

#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

#include "Base/Base_GameMode.h"
#include "Helper/GeneralHelper.h"

#include "Items/Health.h"
#include "Items/HealthRegen.h"
#include "Items/Minigun.h"

// Sets default values
ABase_ItemSpawner::ABase_ItemSpawner() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemTypes.Add(AHealth::StaticClass());
	ItemTypes.Add(AHealthRegen::StaticClass());
	ItemTypes.Add(AMinigun::StaticClass());
}

// Called when the game starts or when spawned
void ABase_ItemSpawner::BeginPlay() {
	Super::BeginPlay();
	if(!HasAuthority()) {
		return;
	}

	GameMode = CastChecked<ABase_GameMode>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ABase_ItemSpawner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if(!HasAuthority()) {
		return;
	}
	RespawnTimer += DeltaTime;

	if(RespawnTimer > ItemRespawnTime && GroundItems.Num() < MaxItems) {
		SpawnRandomItem();
	}
}


ABase_Item* ABase_ItemSpawner::SpawnRandomItem() {
	TSubclassOf<ABase_Item> itemType = GetRandomItemClass();
	return SpawnItem(itemType);
}
ABase_Item* ABase_ItemSpawner::SpawnItem(TSubclassOf<ABase_Item> itemClassToSpawn) {
	if(!HasAuthority()) {
		return NULL;
	}

	for(int i = 0; true; i++) {
		FVector location = GameMode->GetRandomTerrainLocation();
		location.Z = location.Z + 50;
		FActorSpawnParameters spawnParameters = FActorSpawnParameters();
		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ABase_Item* item = GetWorld()->SpawnActor<ABase_Item>(itemClassToSpawn.Get(), FTransform(location), spawnParameters);
		if(IsValid(item)) {
			UE_LOG(LogTemp, Display, TEXT("Base_ItemSpawner: Spawned item %s at location %f/%f/%f"), *item->GetName(), location.X, location.Y, location.Z);
			GroundItems.Add(item);
			RespawnTimer = 0;
			return item;
		}
		if(i == 100) {
			UE_LOG(LogTemp, Error, TEXT("Could not spawn item after %i tries!"), i);
			return NULL;
		}
	}
}
TSubclassOf<ABase_Item> ABase_ItemSpawner::GetRandomItemClass() {
	return ItemTypes[UKismetMathLibrary::RandomInteger(ItemTypes.Num())];
}