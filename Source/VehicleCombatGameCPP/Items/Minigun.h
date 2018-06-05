// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/Base_StreamItem.h"
#include "Minigun.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AMinigun : public ABase_StreamItem
{
	GENERATED_BODY()

	protected:
		void FirePrimary() override;

	public:
		AMinigun();
	
	
};
