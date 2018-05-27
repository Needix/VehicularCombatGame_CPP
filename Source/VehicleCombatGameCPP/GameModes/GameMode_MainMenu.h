#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Blueprint/UserWidget.h"
#include "GameMode_MainMenu.generated.h"

UCLASS()
class VEHICLECOMBATGAMECPP_API AGameMode_MainMenu : public AGameMode {
	GENERATED_BODY()

  public:
	UFUNCTION(BlueprintCallable, Category = "UMG Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> newWidgetClass);

  protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartingWidgetClass;

	UPROPERTY()
	UUserWidget *CurrentWidget;
};