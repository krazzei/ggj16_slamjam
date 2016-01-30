// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamGameMode.h"
#include "ggj16_slamjamCharacter.h"

Aggj16_slamjamGameMode::Aggj16_slamjamGameMode()
{
	// set default pawn class to our character
	DefaultPawnClass = CharacterPawnClass;
}

void Aggj16_slamjamGameMode::ResetLevel()
{
	auto MapName = GetWorld()->GetMapName();
	
	UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName));
}
