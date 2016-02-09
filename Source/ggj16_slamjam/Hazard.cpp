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

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
}

void AHazard::BeginPlay()
{
	Super::BeginPlay();

	CurrentLevel = FName(*GetWorld()->GetMapName());
}

void AHazard::BeginOverlap(AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<Aggj16_slamjamCharacter>(Other);
	
	if (Character && Character->GetMoveState() != WinState &&
		!(WinState == ECharMoveState::SideStepLeft || WinState == ECharMoveState::SideStepRight))
	{
		UGameplayStatics::OpenLevel(GetWorld(), CurrentLevel);
	}
}

void AHazard::EndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Character && Character->GetMoveState() != WinState)
	{
		UGameplayStatics::OpenLevel(GetWorld(), CurrentLevel);
	}

	Character = NULL;
}

void AHazard::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Character && Character->GetMoveState() == ECharMoveState::Idle)
	{
		UGameplayStatics::OpenLevel(GetWorld(), CurrentLevel);
	}
}
