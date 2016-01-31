// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamCharacter.h"
#include "Hazard.h"

AHazard::AHazard()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AHazard::BeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AHazard::EndOverlap);
	
	RootComponent = Trigger;
}

void AHazard::BeginOverlap(AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Side steps need to be checked when the player leaves.
	if (WinState == ECharMoveState::SideStepLeft || WinState == ECharMoveState::SideStepRight)
	{
		return;
	}
	
	auto Character = Cast<Aggj16_slamjamCharacter>(Other);
	
	if (Character && Character->GetMoveState() != WinState)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetMapName()));
	}
}

void AHazard::EndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(WinState != ECharMoveState::SideStepLeft || WinState != ECharMoveState::SideStepRight)
	{
		return;
	}
	
	auto Character = Cast<Aggj16_slamjamCharacter>(Other);
	
	if (Character && Character->GetMoveState() != WinState)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetMapName()));
	}
}
