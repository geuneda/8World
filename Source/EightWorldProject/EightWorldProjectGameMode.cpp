// Copyright Epic Games, Inc. All Rights Reserved.

#include "EightWorldProjectGameMode.h"
#include "UObject/ConstructorHelpers.h"

AEightWorldProjectGameMode::AEightWorldProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Script/Engine.Blueprint'/Game/PalWorld/Blueprints/Player/BP_Player.BP_Player'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
