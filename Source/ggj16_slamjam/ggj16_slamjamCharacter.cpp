// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"



DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);
//////////////////////////////////////////////////////////////////////////
// Aggj16_slamjamCharacter

Aggj16_slamjamCharacter::Aggj16_slamjamCharacter()
{
	// Setup the assets
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationAsset;
		/*FConstructorStatics()
			: RunningAnimationAsset(TEXT("/Game/2dSideScroller/Sprites/RunningAnimation.RunningAnimation"))
			, IdleAnimationAsset(TEXT("/Game/2dSideScroller/Sprites/IdleAnimation.IdleAnimation"))
		{
		}*/

		FConstructorStatics()
			: RunningAnimationAsset(TEXT("/Game/Character/WalkingAnimation.WalkingAnimation"))
			, IdleAnimationAsset(TEXT("/Game/Character/Idle.Idle"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	RunningAnimation = ConstructorStatics.RunningAnimationAsset.Get();
	IdleAnimation = ConstructorStatics.IdleAnimationAsset.Get();
	GetSprite()->SetFlipbook(IdleAnimation);

	// Use only Roll from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);
	GetCapsuleComponent()->SetLockedAxis(EDOFMode::XYPlane);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(-90.0f, -90.0f, 0.0f);
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->bAbsoluteRotation = true;
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XY plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

// 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
// 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
// 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
// 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
// 	TextComponent->AttachTo(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	bCanMove = true;
	bStopMoving = true;
	moveState = ECharMoveState::Idle;
	prevMoveState = ECharMoveState::Idle;
	moveDistance = 110;

}

void Aggj16_slamjamCharacter::BeginPlay()
{
	Super::BeginPlay();

	//FVector actorPos = GetActorLocation();
	//float xPos = actorPos.X;
	//float yPos = actorPos.Y;

	//xPos = FMath::RoundToFloat(actorPos.X / 50) * 100;
	//yPos = FMath::RoundToFloat(actorPos.Y / 50) * 100;

	//SetActorLocation(FVector(xPos, yPos, 10.0f));

}

//////////////////////////////////////////////////////////////////////////
// Animation

void Aggj16_slamjamCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeed = PlayerVelocity.Size();

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeed > 0.0f) ? RunningAnimation : IdleAnimation;

	//if (MoveList.Num() <= 0)
	//{
	//	DesiredAnimation = IdleAnimation;
	//}
	//else
	//{

		switch (moveState)
		{
		case ECharMoveState::Idle:
			DesiredAnimation = IdleAnimation;
			break;
		case ECharMoveState::MoveDown:
		case ECharMoveState::MoveLeft:
		case ECharMoveState::MoveRight:
		case ECharMoveState::MoveUp:
			DesiredAnimation = RunningAnimation;
			break;
		case ECharMoveState::Jump:

			break;
		default:
			break;
		}
//	}

	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void Aggj16_slamjamCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacter(DeltaSeconds);	
}


//////////////////////////////////////////////////////////////////////////
// Input

void Aggj16_slamjamCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	APlayerController* pc = Cast<APlayerController>(GetController());
	pc->bShowMouseCursor = true;
}

void Aggj16_slamjamCharacter::MoveRight()
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	if (bCanMove)
	{
		prevMoveState = moveState;
		moveState = ECharMoveState::MoveRight;
		facingDirection = ECharMoveState::MoveRight;
		moveTarget = GetActorLocation() + FVector(1.0, 0.0, 0.0) * moveDistance;
		bStopMoving = false;
		bCanMove = false;
	}
}

void Aggj16_slamjamCharacter::MoveUp()
{
	if (bCanMove)
	{
		prevMoveState = moveState;
		moveState = ECharMoveState::MoveUp;
		facingDirection = ECharMoveState::MoveUp;
		moveTarget = GetActorLocation() + FVector(0.0, -1.0, 0.0) * moveDistance;
		bStopMoving = false;
		bCanMove = false;
	}
}

void Aggj16_slamjamCharacter::MoveDown()
{
	if (bCanMove)
	{
		prevMoveState = moveState;
		moveState = ECharMoveState::MoveDown;
		facingDirection = ECharMoveState::MoveDown;
		moveTarget = GetActorLocation() + FVector(0.0, 1.0, 0.0) * moveDistance;
		bStopMoving = false;
		bCanMove = false;
	}
}

void Aggj16_slamjamCharacter::MoveLeft()
{
	if (bCanMove)
	{
		prevMoveState = moveState;
		moveState = ECharMoveState::MoveLeft;
		facingDirection = ECharMoveState::MoveLeft;
		moveTarget = GetActorLocation() + FVector(-1.0, 0.0, 0.0) * moveDistance;
		bStopMoving = false;
		bCanMove = false;
	}
}

void Aggj16_slamjamCharacter::Jump()
{
	
}

void Aggj16_slamjamCharacter::Roll()
{

}

void Aggj16_slamjamCharacter::SideStep()
{

}



void Aggj16_slamjamCharacter::Pickup(AItemPickup* ItemPickup)
{
	switch (ItemPickup->ItemType)
	{
		case EItemType::Jump:
			MoveQueue.Add(ECharMoveState::Jump);
			break;
		case EItemType::Roll:
			MoveQueue.Add(ECharMoveState::Roll);
			break;
		case EItemType::SideStepLeft:
			MoveQueue.Add(ECharMoveState::SideStepLeft);
			break;
		case EItemType::SideStepRight:
			MoveQueue.Add(ECharMoveState::SideStepRight);
			break;
		case EItemType::Key:
			KeyAmount++;
			break;
		default:
			break;
	}
}

void Aggj16_slamjamCharacter::UpdateCharacter(float DeltaSeconds)
{
	// Update animation to match the motion
	UpdateAnimation();

	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
//		FVector FMath::Lerp(GetTransform().GetLocation(), GetTransform().GetLocation() + FVector(10.0f, 0.0f, 0.0f), )
//		FRotator DesiredRotation;  // Controller->GetControlRotation();
		FVector actorPos = GetActorLocation();
		if (facingDirection == ECharMoveState::MoveDown)
		{
			Controller->SetControlRotation(FRotator::MakeFromEuler(FVector(0.0f, 0.0f, 0.0f)));
		}
		else if (facingDirection == ECharMoveState::MoveLeft)
		{
			Controller->SetControlRotation(FRotator(0.0f, 90.0f, 0.0f));
		}
		else if (facingDirection == ECharMoveState::MoveRight)
		{
			Controller->SetControlRotation(FRotator(0.0f, -90.0f, 0.0f));
		}
		else if (facingDirection == ECharMoveState::MoveUp)
		{
			Controller->SetControlRotation(FRotator(0.0f, 180.0f, 0.0f));
		}

		if (FVector::PointsAreNear(actorPos, moveTarget, 0.01) && !bStopMoving)
		{
			SetActorLocation(moveTarget);
			bCanMove = true;
			bStopMoving = true;
			moveState = ECharMoveState::Idle;
		}
		else if(!bStopMoving)
		{
			FVector newPos = FMath::Lerp(GetActorLocation(), moveTarget, 0.5);
			SetActorLocation(newPos);
		}

		/*if (TravelDirectionX < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 180.0f, 180.0f));
		}
		else if (TravelDirectionX > 0.0f)
		{
			Controller->SetControlRotation(FRotator(90.0f, 180.0f, 90.0f));
		}*/
	}
}

uint8 Aggj16_slamjamCharacter::GetKeyAmount() const
{
	return KeyAmount;
}

bool Aggj16_slamjamCharacter::ConsumeKey()
{
	if (KeyAmount > 0)
	{
		KeyAmount--;
		return true;
	}
	
	return false;
}

TArray<uint8> Aggj16_slamjamCharacter::GetActions()
{
	TArray<uint8> Actions = TArray<uint8>();
	
	auto ActionList = MoveQueue.GetActions();
	
	for (auto i = 0; i < ActionList.Num(); ++i)
	{
		Actions.Add((uint8)ActionList[i]);
	}
	
	return Actions;
}
