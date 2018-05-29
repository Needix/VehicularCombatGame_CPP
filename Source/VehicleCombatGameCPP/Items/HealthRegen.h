// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Base_TimerItem.h"
#include "HealthRegen.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AHealthRegen : public ABase_TimerItem
{
	GENERATED_BODY()
	
	private:
		float RegenAmountPerSecond = 4;
	
	protected:
		virtual void UseTick(float delta) override;
	
	public:
		AHealthRegen();
};
