// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/EngineBaseTypes.h"
#include "OnlineSubsystemUtils.h"
#include "MyGameInstance.generated.h"

/**
 *
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API UMyGameInstance : public UGameInstance {
	GENERATED_BODY()

	FOnCreateSessionCompleteDelegate CreateSessionCompleteD;
	FDelegateHandle CreateSessionCompleteDH;
	FOnlineSessionSettings SessionSettings;
	FName SessionName = "VCG_Test";

	void CreateSessionComplete(FName SessionName, bool bWasSuccessful);
	//void JoinSessionComplete

	public:
		void Init() override;
		//void HandleNetworkError(ENetworkFailure::Type FailureType, bool bIsServer) override;
		void Shutdown() override;
		//void HandleTravelError(ETravelFailure::Type FailureType) override;
		bool Host(int32 index);
		bool Join(int32 index);
};
