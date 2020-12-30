// Copyright Epic Games, Inc. All Rights Reserved.

#include "WitchGameGameMode.h"
#include "WitchGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWitchGameGameMode::AWitchGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
