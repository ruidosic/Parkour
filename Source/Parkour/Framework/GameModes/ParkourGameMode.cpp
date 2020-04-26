// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourGameMode.h"
#include "UObject/ConstructorHelpers.h"


AParkourGameMode::AParkourGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PawnClass(TEXT("/Game/Parkour/MainCharacter/Blueprints/BP_MainCharacter"));
	if (PawnClass.Succeeded())
	{
		DefaultPawnClass = PawnClass.Class;
	}
}