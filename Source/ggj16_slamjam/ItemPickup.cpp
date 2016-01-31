// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamCharacter.h"
#include "ItemPickup.h"

// Sets default values
AItemPickup::AItemPickup()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::BeginOverlap);
}

void AItemPickup::BeginOverlap(AActor *Other, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	auto Character = Cast<Aggj16_slamjamCharacter>(Other);
	if (Character)
	{
		Character->Pickup(this);
		Destroy();
	}
}
