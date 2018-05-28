// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameSession.h"


bool AMyGameSession::GetSessionJoinability(FName InSessionName, FJoinabilitySettings& OutSettings) {
	// TODO: Override this to disable multiplayer auto join: https://answers.unrealengine.com/questions/215018/stop-players-from-connecting-to-an-online-session.html
	return Super::GetSessionJoinability(InSessionName, OutSettings);
}


