// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionQueue.generated.h"

UENUM(BlueprintType)
enum class ECharMoveState : uint8
{
	Jump,
	Roll,
	SideStepLeft,
	SideStepRight,
	Idle,
	MoveRight,
	MoveLeft,
	MoveUp,
	MoveDown
};

USTRUCT()
struct FActionQueue
{
	GENERATED_USTRUCT_BODY()
	
public:
	FActionQueue();
	~FActionQueue();
	
	void Add(ECharMoveState Action);
	
	TArray<ECharMoveState> GetActions() const;
	
private:
	TArray<ECharMoveState> MoveList;
};
