// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "SoundDefinitions.h"
#include "RRUserSettings.h"


void URRUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
	SetAudioVolume();
}


void URRUserSettings::SetAudioVolume()
{
	FAudioDevice* Device = GEngine->GetMainAudioDevice();
	if (Device)
	{
		for (auto It = Device->SoundClasses.CreateConstIterator(); It; ++It)
		{
			USoundClass* SoundClass = It.Key();
			if (SoundClass && SoundClass->GetFullName().Find("Master") != INDEX_NONE)
			{
				SoundClass->Properties.Volume = SoundVolume;
			}
		}
	}
}

