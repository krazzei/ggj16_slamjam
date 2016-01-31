// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PaperCharacter.h"
#include "ItemPickup.h"
#include "ActionQueue.h"
#include "ggj16_slamjamCharacter.generated.h"

// This class is the default character for ggj16_slamjam, and it is responsible for all
// physical interaction between the player and the world.
//
//   The capsule component (inherited from ACharacter) handles collision with the world
//   The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
//   The Sprite component (inherited from APaperCharacter) handles the visuals

class UTextRenderComponent;

UCLASS(config=Game)
class Aggj16_slamjamCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UTextRenderComponent* TextComponent;
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	uint8 KeyAmount = 0;
	
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* RollAnimation;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void UpdateCharacter(float DeltaSeconds);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	Aggj16_slamjamCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	bool bCanMove;
	bool bStopMoving;

	virtual void BeginPlay() override;

	FVector moveTarget;


	ECharMoveState moveState;
	ECharMoveState prevMoveState;

	UPROPERTY(EditAnywhere)
	ECharMoveState facingDirection;

	UPROPERTY(EditDefaultsOnly)
	float moveDistance;

	TArray<ECharMoveState> MoveList;
	
	FActionQueue MoveQueue;

	int32 MoveIndex = 0;

	UFUNCTION(BlueprintCallable, Category = Movement)
	void MoveRight();
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void MoveUp();
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void MoveDown();
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	void MoveLeft();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void Jump();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void Roll();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SideStepLeft();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SideStepRight();

	void FinishedMove();

	void PerformNextMove();

	FVector GetMoveDirection();

	UFUNCTION(BlueprintCallable, Category = Pickups)
	void Pickup(AItemPickup* ItemPickup);

	UFUNCTION(BlueprintCallable, Category = Keys)
	uint8 GetKeyAmount() const;
	
	UFUNCTION(BlueprintCallable, Category = Keys)
	bool ConsumeKey();
	
	UFUNCTION(BlueprintCallable, Category = Action)
	TArray<uint8> GetActions();
};
