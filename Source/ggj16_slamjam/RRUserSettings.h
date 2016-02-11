// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameUserSettings.h"
#include "RRUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class GGJ16_SLAMJAM_API URRUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void SetAudioVolume();


	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;

	float GetSoundVolume() const
	{
		return SoundVolume;
	}

	void SetSoundVolume(float InVolume)
	{
		SoundVolume = InVolume;
	}


protected:

	UPROPERTY()
	float SoundVolume;

};
