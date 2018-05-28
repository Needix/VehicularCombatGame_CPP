// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Gameplay.h"

//#include "Runtime/UMG/Public/UMG.h"
//#include "Runtime/UMG/Public/UMGStyle.h"
//#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
//#include "Runtime/UMG/Public/IUMGModule.h"
//#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#include "Base/Base_GameMode.h"
#include "AllLevel/Singleton.h"

void UWidget_Gameplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	if(Teams.Num() != ABase_GameMode::MaxTeams) {
		TArray<AActor *> teams;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeam::StaticClass(), teams);
		Teams = SortTeamsByName(teams);
	}
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TArray<ATeam*> UWidget_Gameplay::SortTeamsByName(TArray<AActor*> teams) {
	TArray<ATeam*> result;
	for(AActor* actor : teams) {
		ATeam* team1 = CastChecked<ATeam>(actor);
		UE_LOG(LogTemp, Warning, TEXT("Widget_GamePlay: SortTeamsByName: Adding %s at %i (this has to be logged, else the algorithm doesnt work...)"), *team1->GetName(), result.Num()); // Need: Order will not be correct without this. DONT ask me why....
		result.Add(team1);
	}
	return result;
}

