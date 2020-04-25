// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ParkourGameState.generated.h"

/**
 * 
 */
UCLASS()
class PARKOUR_API AParkourGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	virtual float GetServerWorldTimeSeconds() const override;
};
