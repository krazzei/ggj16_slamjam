// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamCharacter.h"
#include "Hazard.h"

AHazard::AHazard()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AHazard::BeginOverlap);
	
	RootComponent = Trigger;
}

void AHazard::BeginOverlap(AActor *Other, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	auto Character = Cast<Aggj16_slamjamCharacter>(Other);
	
	if (Character && Character->GetMoveState() != WinState)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetMapName()));
	}
}
