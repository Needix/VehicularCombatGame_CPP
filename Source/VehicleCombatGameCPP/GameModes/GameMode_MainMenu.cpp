#include "GameMode_MainMenu.h"

void AGameMode_MainMenu::BeginPlay() {
	Super::BeginPlay();

	ChangeMenuWidget(StartingWidgetClass);
}

void AGameMode_MainMenu::ChangeMenuWidget(TSubclassOf<UUserWidget> newWidgetClass) {
	if (CurrentWidget != nullptr) {
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (newWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), newWidgetClass);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport();
		}
	}
}