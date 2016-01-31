// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ItemPickup.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Jump,
	Roll,
	SideStepLeft,
	SideStepRight,
	Key,
};

UCLASS()
class GGJ16_SLAMJAM_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

	UPROPERTY(EditDefaultsOnly)
	EItemType ItemType;
private:
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* Trigger;
	
	UFUNCTION()
	void BeginOverlap(AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
