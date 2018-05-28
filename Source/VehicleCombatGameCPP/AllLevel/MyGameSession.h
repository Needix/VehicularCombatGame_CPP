// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "MyGameSession.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API AMyGameSession : public AGameSession
{
	GENERATED_BODY()
	
	
	virtual bool GetSessionJoinability(FName InSessionName, FJoinabilitySettings& OutSettings);
	
	
};
