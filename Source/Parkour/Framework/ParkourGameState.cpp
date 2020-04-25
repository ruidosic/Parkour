// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourGameState.h"
#include "ParkourPlayerController.h"
#include "Engine/GameInstance.h"

float AParkourGameState::GetServerWorldTimeSeconds() const
{
	AParkourPlayerController* PC = Cast<AParkourPlayerController>(GetGameInstance()->GetFirstLocalPlayerController(GetWorld()));
	
	if (PC)
	{
		return PC->GetServerTime();
	}
	
	else
	{
		return GetWorld()->GetTimeSeconds();
	}
}