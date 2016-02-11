// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "RRUserSettings.h"
#include "MainOptionsMenu.h"


UMainOptionsMenu::UMainOptionsMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}


void UMainOptionsMenu::SetGameVolume(float volume)
{
	GameVolume = volume;
	URRUserSettings* UserSettings = Cast<URRUserSettings>(GEngine->GetGameUserSettings());

	if (UserSettings)
	{
		UserSettings->SetSoundVolume(GameVolume);
		UserSettings->ApplySettings(false);
	}
}

float UMainOptionsMenu::GetGameVolume()
{
	return GameVolume;
	URRUserSettings* UserSettings = Cast<URRUserSettings>(GEngine->GetGameUserSettings());

	if (UserSettings)
	{
		UserSettings->SetSoundVolume(50);
		UserSettings->ApplySettings(false);
	}
}
