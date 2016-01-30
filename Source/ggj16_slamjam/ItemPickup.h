// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ItemPickup.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Jump
};


UCLASS()
class GGJ16_SLAMJAM_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditDefaultsOnly)
	EItemType ItemType;
	
};
