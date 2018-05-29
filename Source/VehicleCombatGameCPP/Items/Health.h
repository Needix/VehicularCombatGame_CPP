// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Base_UseItem.h"
#include "Health.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AHealth : public ABase_UseItem
{
	GENERATED_BODY()
	
	private:
		float HealAmount = 40;
	
	
	protected:
		virtual void Use() override;

	public:
		AHealth();
	
};
