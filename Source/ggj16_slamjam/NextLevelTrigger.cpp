// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamCharacter.h"
#include "NextLevelTrigger.h"

// Sets default values
ANextLevelTrigger::ANextLevelTrigger()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ANextLevelTrigger::BeginOverlap);
}

void ANextLevelTrigger::BeginOverlap(AActor *Other, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	auto Character = Cast<Aggj16_slamjamCharacter>(Other);
	if (Character)
	{
		UGameplayStatics::OpenLevel(GetWorld(), NextLevel);
	}
}
