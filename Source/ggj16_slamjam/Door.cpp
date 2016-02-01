// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamCharacter.h"
#include "Door.h"

// Sets default values
ADoor::ADoor()
{
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	
	RootComponent = Collider;
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADoor::BeginOverlap);
	Trigger->AttachTo(Collider);
}

void ADoor::BeginOverlap(AActor *Other, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	auto Character = Cast<Aggj16_slamjamCharacter>(Other);
	if (Character && Character->ConsumeKey())
	{
		Destroy();
	}
}
