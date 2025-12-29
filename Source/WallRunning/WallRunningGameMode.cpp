// Copyright Epic Games, Inc. All Rights Reserved.

#include "WallRunningGameMode.h"
#include "WallRunningCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWallRunningGameMode::AWallRunningGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
