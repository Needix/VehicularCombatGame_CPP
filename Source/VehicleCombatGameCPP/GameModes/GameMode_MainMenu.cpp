#include "GameMode_MainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"

AGameMode_MainMenu::AGameMode_MainMenu() {
	static ConstructorHelpers::FObjectFinder<USoundCue> backgroundMusicObject(TEXT("/Game/StarterContent/Audio/Starter_Music_Cue.Starter_Music_Cue"));
	UGameplayStatics::PlaySound2D(GetWorld(), backgroundMusicObject.Object);
}

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