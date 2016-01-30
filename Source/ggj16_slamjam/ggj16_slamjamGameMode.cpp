// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamGameMode.h"
#include "ggj16_slamjamCharacter.h"

Aggj16_slamjamGameMode::Aggj16_slamjamGameMode()
{
	// set default pawn class to our character
	DefaultPawnClass = Aggj16_slamjamCharacter::StaticClass();	
}
