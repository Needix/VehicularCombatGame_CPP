// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

#include "Base/Base_Item.h"

#include "Base_UseItem.generated.h"

/**
 * A item which gets used once for every click
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API ABase_UseItem : public ABase_Item
{
	GENERATED_BODY()
	
	private:
		bool WasUsed;

	protected:
		virtual void Use();
	
	public:
		virtual void Tick(float delta) override;
};
