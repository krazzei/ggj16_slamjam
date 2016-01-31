// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NextLevelTrigger.generated.h"

UCLASS()
class GGJ16_SLAMJAM_API ANextLevelTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	ANextLevelTrigger();
	
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* Trigger;
	
	UPROPERTY(EditDefaultsOnly)
	FName NextLevel;
	
private:
	UFUNCTION()
	void BeginOverlap(AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
