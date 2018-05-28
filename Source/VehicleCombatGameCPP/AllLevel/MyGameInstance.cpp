// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"


void UMyGameInstance::Init() {
	IOnlineSubsystem* OSInst = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInst = OSInst->GetSessionInterface();

	CreateSessionCompleteD = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMyGameInstance::CreateSessionComplete);
	CreateSessionCompleteDH = SessionInst->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteD);
}
void UMyGameInstance::Shutdown() {

}

//void UMyGameInstance::HandleNetworkError(ENetworkFailure::Type FailureType, bool bIsServer) {
//
//}
//void UMyGameInstance::HandleTravelError(ETravelFailure::Type FailureType) {
//
//}

bool UMyGameInstance::Host(int32 index) {
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bAllowJoinInProgress = true;
	//SessionSettings.bUsesPresence = true;   // What is presence?
	SessionSettings.bShouldAdvertise = true;

	IOnlineSubsystem* OSInst = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInst = OSInst->GetSessionInterface();

	// If session isn't created then one might already exist so try to destroy it.	
	if (SessionInst->CreateSession(0, SessionName, SessionSettings)) {
		SessionInst->StartSession(SessionName);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Successfully created session...");
		UE_LOG(LogTemp, Warning, TEXT("Successfully created session..."));
		return true;
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Could not create session. Trying to destroy old sesion...");
		UE_LOG(LogTemp, Warning, TEXT("Could not create session. Trying to destroy old sesion..."));

		if (SessionInst->DestroySession(SessionName)) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Successully destroyed session!");
			UE_LOG(LogTemp, Warning, TEXT("Successully destroyed session!"));
			Host(index);
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Could not delete session!");
			UE_LOG(LogTemp, Warning, TEXT("Could not delete session!"));
		}
		return false;
	}
}

bool UMyGameInstance::Join(int32 index) {
	IOnlineSubsystem* OSInst = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInst = OSInst->GetSessionInterface();
	
	UE_LOG(LogTemp, Warning, TEXT("Trying to find sessions..."));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Trying to find sessions...");

	TSharedRef <FOnlineSessionSearch> SearchSettings(new FOnlineSessionSearch());
	//SearchSettings.Get().
	SearchSettings.Get().bIsLanQuery = true;
	SearchSettings.Get().MaxSearchResults = 10;
	//SearchSettings.Get().QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInst->FindSessions(0, SearchSettings);

	UE_LOG(LogTemp, Warning, TEXT("Found %i sessions!"), SearchSettings->SearchResults.Num());

	if(SearchSettings->SearchResults.Num() != 0) {
		FOnlineSessionSearchResult &SearchResult = SearchSettings->SearchResults[0];

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Trying to join server...");
		UE_LOG(LogTemp, Warning, TEXT("Trying to join server..."));

		bool joinSuccess = SessionInst->JoinSession(0, SessionName, SearchResult);

		UE_LOG(LogTemp, Warning, TEXT("Joining server was %ssuccessful"), joinSuccess ? "" : "not ");
	}
	return true;
}

void UMyGameInstance::CreateSessionComplete(FName SessionName, bool bWasSuccessful) {
	if (bWasSuccessful) {
		UE_LOG(LogTemp, Warning, TEXT("Session creation successfully completed"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Session creation successfully completed");

		UGameplayStatics::OpenLevel(GetWorld(), "Desert", true, "listen");
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Session creation was not successful!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Session creation was not successful!");
	}
}


