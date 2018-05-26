// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
//#include "Runtime/UMG/Public/UMG.h"
//#include "Runtime/UMG/Public/UMGStyle.h"
//#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
//#include "Runtime/UMG/Public/IUMGModule.h"
//#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "GameModes/Team.h"

#include "Widget_Gameplay.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLECOMBATGAMECPP_API UWidget_Gameplay : public UUserWidget
{
	GENERATED_BODY()
	
	private:
		TArray<ATeam*> Teams;
	
	private: 
		UFUNCTION(BlueprintCallable, Category = Getter)
		TArray<ATeam*> GetTeams() { return Teams; }

		TArray<ATeam*> SortTeamsByName(TArray<AActor*> teams);

	public:
		virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
