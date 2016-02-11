// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MainOptionsMenu.generated.h"

/**
 * 
 */
UCLASS()
class GGJ16_SLAMJAM_API UMainOptionsMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UMainOptionsMenu(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	void SetGameVolume(float volume);

	UFUNCTION(BlueprintCallable, Category = "Game Options")
	float GetGameVolume();

protected:
	UPROPERTY(EditAnywhere, Category = "Game Options")
	float GameVolume;

};
