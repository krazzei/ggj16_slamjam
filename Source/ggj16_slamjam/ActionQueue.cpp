// Fill out your copyright notice in the Description page of Project Settings.

#include "ggj16_slamjam.h"
#include "ActionQueue.h"

FActionQueue::FActionQueue()
{
	MoveList = TArray<ECharMoveState>();
}

FActionQueue::~FActionQueue()
{
}

void FActionQueue::Add(ECharMoveState Action)
{
	GLog->Log("Adding: " + FString::FromInt((int32)Action));
	
	if (MoveList.Num() == 3)
	{
		auto Last = MoveList.Last();
		MoveList.Remove(Last);
	}
	
	// This is no longer reverse.
	auto Reverse = TArray<ECharMoveState>();
	
	for (int32 i = 0; i < MoveList.Num(); ++i)
	{
		Reverse.Add(MoveList[i]);
	}
	
	MoveList.Empty();
	
	MoveList.Add(Action);
	
	for (int32 i = 0; i < Reverse.Num(); ++i)
	{
		GLog->Log(FString::FromInt((int32)Reverse[i]));
		MoveList.Add(Reverse[i]);
	}
}

TArray<ECharMoveState> FActionQueue::GetActions() const
{
	return MoveList;
}
