// Fill out your copyright notice in the Description page of Project Settings.

#include "ParkourPlayerController.h"
#include "GameFramework/GameState.h"

void AParkourPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		Server_RequestServerTime(this, GetWorld()->GetTimeSeconds());
	}
}

void AParkourPlayerController::Client_ReportServerTime_Implementation(float RequestWorldTime, float ServerTimeValue)
{
	// Apply the round-trip request time to the server's         
	// reported time to get the up-to-date server time

	float RoundTripTime = GetWorld()->GetTimeSeconds() - RequestWorldTime;

	float AdjustedTime = ServerTimeValue + (RoundTripTime * 0.5f);

	ServerTime = AdjustedTime;
}

void AParkourPlayerController::Server_RequestServerTime_Implementation(APlayerController* Requester, float RequestWorldTime)
{
	float ServerTimeValue = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	Client_ReportServerTime(RequestWorldTime, ServerTimeValue);
}

bool AParkourPlayerController::Server_RequestServerTime_Validate(APlayerController* Requester, float RequestWorldTime)
{
	return true;
}