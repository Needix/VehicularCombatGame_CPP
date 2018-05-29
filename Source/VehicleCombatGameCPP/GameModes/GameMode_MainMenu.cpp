#include "GameMode_MainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Pawn.h"
#include "Player/Player_Controller.h"

AGameMode_MainMenu::AGameMode_MainMenu() {
	static ConstructorHelpers::FObjectFinder<USoundCue> backgroundMusicObject(TEXT("/Game/StarterContent/Audio/Starter_Music_Cue.Starter_Music_Cue"));
	UGameplayStatics::PlaySound2D(GetWorld(), backgroundMusicObject.Object);

	DefaultPawnClass = APawn::StaticClass();
	PlayerControllerClass = APlayer_Controller::StaticClass();
}

void AGameMode_MainMenu::BeginPlay() {
	Super::BeginPlay();
}