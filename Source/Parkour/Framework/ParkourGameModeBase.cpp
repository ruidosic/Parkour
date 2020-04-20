// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ParkourGameModeBase.h"
#include "UObject/ConstructorHelpers.h"

AParkourGameModeBase::AParkourGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PawnClass(TEXT("/Game/Parkour/MainCharacter/Blueprints/BP_MainCharacter"));
	if (PawnClass.Succeeded())
	{
		DefaultPawnClass = PawnClass.Class;
	}
}