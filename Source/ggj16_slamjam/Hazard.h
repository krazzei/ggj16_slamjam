// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ActionQueue.h"
#include "Hazard.generated.h"

UCLASS()
class GGJ16_SLAMJAM_API AHazard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHazard();
	
	UBoxComponent* Trigger;
	
	UPROPERTY(EditAnywhere)
	ECharMoveState WinState;
	
private:
	UFUNCTION()
	void BeginOverlap(AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
