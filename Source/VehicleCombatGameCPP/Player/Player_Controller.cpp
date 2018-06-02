// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Curves/CurveFloat.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player_DrivePawn.h"

APlayer_Controller::APlayer_Controller() {
	Cameras.Add(FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1, 1, 1)));
	Cameras.Add(FTransform(FRotator(10, 0, 0), FVector(-125, 0, 0), FVector(1, 1, 1)));
	Cameras.Add(FTransform(FRotator(10, 0, 0), FVector(-200, 0, 0), FVector(1, 1, 1)));

	SetupCameraTransitionTimeline();
}

void APlayer_Controller::BeginPlay() {
	TArray<AActor *> singletons;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASingleton::StaticClass(), singletons);

	check(singletons.Num() == 1);
	Singleton = CastChecked<ASingleton>(singletons[0]);
}

void APlayer_Controller::Tick(float deltaTime) {
	APawn *pawn = GetPawn();
	if (!IsValid(pawn)) {
		RespawnTimer += deltaTime;
	} else {
		RespawnTimer = 0;
	}

	if (IsValid(CameraTransitionTimeline)) {
		CameraTransitionTimeline->TickComponent(deltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}

void APlayer_Controller::ChangeMenuWidget(TSubclassOf<UUserWidget> newWidgetClass) {
	if (CurrentWidget != nullptr) {
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	bShowMouseCursor = newWidgetClass != nullptr;

	if (newWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), newWidgetClass);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport(100);
		}
	}
}

void APlayer_Controller::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &APlayer_Controller::OnToggleCamera);

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &APlayer_Controller::OnPauseMenu);
}

void APlayer_Controller::SetupCameraTransitionTimeline() {
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/VehicularCombatGame/Curves/Linear.Linear"));

	CameraTransitionTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));

	FOnTimelineFloat callback;
	FOnTimelineEventStatic onFinishedCallback;

	callback.BindUFunction(this, FName{TEXT("CameraTransitionTimelineCallback")});
	CameraTransitionTimeline->AddInterpFloat(Curve.Object, callback);

	onFinishedCallback.BindUFunction(this, FName{TEXT("CameraTransitionTimelineFinishedCallback")});
	CameraTransitionTimeline->SetTimelineFinishedFunc(onFinishedCallback);

	CameraTransitionTimeline->RegisterComponent();
}
void APlayer_Controller::OnToggleCamera() {
	APlayer_DrivePawn *pawn = CastChecked<APlayer_DrivePawn>(GetPawn());
	if (!IsValid(pawn)) {
		return;
	}

	OldCameraTransform = pawn->GetCamera()->GetRelativeTransform();

	int newIndex = CurrentCamera + 1;

	if (newIndex >= Cameras.Num()) {
		newIndex = 0;
	}

	CurrentCamera = newIndex;

	if (IsValid(CameraTransitionTimeline)) {
		CameraTransitionTimeline->PlayFromStart();
	}
}
void APlayer_Controller::CameraTransitionTimelineCallback(float val) {
	FTransform transform = UKismetMathLibrary::TLerp(OldCameraTransform, Cameras[CurrentCamera], val);

	APlayer_DrivePawn *pawn = CastChecked<APlayer_DrivePawn>(GetPawn());
	pawn->GetCamera()->SetRelativeLocationAndRotation(transform.GetLocation(), transform.GetRotation());
}
void APlayer_Controller::CameraTransitionTimelineFinishedCallback() {
	APlayer_DrivePawn *pawn = CastChecked<APlayer_DrivePawn>(GetPawn());
	if (!IsValid(pawn)) {
		return;
	}

	pawn->EnableIncarView(CurrentCamera == 0);
}

void APlayer_Controller::OnPauseMenu() {
	SetInputMode(FInputModeUIOnly());
	ChangeMenuWidget(Singleton->PauseMenuWidgetClass);
}