// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class GGJ16_SLAMJAM_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* Trigger;
	
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* Collider;
	
private:
	UFUNCTION()
	void BeginOverlap(AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
