// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

#include "Base/Base_Item.h"

#include "Base_TimerItem.generated.h"

/**
 * A item which gets enabled after a click and stays active for a certain amount of time
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API ABase_TimerItem : public ABase_Item
{
	GENERATED_BODY()
	
	private:
		bool IsEnabled = false;
	
	protected:
		float Timer = 0;

	protected:
		virtual void UseTick(float delta);
	
	public:
		void Tick(float delta) override;
};
