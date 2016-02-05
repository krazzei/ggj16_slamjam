// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ggj16_slamjam.h"
#include "ggj16_slamjamCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "DrawDebugHelpers.h"

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
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RollAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> SideAnimationAsset;
		/*FConstructorStatics()
			: RunningAnimationAsset(TEXT("/Game/2dSideScroller/Sprites/RunningAnimation.RunningAnimation"))
			, IdleAnimationAsset(TEXT("/Game/2dSideScroller/Sprites/IdleAnimation.IdleAnimation"))
		{
		}*/

		FConstructorStatics()
			: RunningAnimationAsset(TEXT("/Game/Character/WalkingAnimation.WalkingAnimation"))
			, IdleAnimationAsset(TEXT("/Game/Character/Idle.Idle"))
			, JumpAnimationAsset(TEXT("/Game/Character/JumpAnimation.JumpAnimation"))
			, RollAnimationAsset(TEXT("/Game/Character/RollAnimation.RollAnimation"))
			, SideAnimationAsset(TEXT("/Game/Character/SideStepAnimation.SideStepAnimation"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	SideStepAnimation = ConstructorStatics.SideAnimationAsset.Get();
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
	GetCapsuleComponent()->SetConstraintMode(EDOFMode::XZPlane);

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
	
	GetSprite()->SetIsReplicated(false);
	bReplicates = false;

	bIsMoving = false;
	bCanMove = true;
	bStopMoving = true;
	moveState = ECharMoveState::Idle;
	prevMoveState = ECharMoveState::Idle;
}

void Aggj16_slamjamCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetMoveTarget(GetActorLocation());
}

//////////////////////////////////////////////////////////////////////////
// Animation

void Aggj16_slamjamCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeed = PlayerVelocity.Size();

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation;

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
		DesiredAnimation = JumpAnimation;
		break;
	case ECharMoveState::Roll:
		DesiredAnimation = RollAnimation;
		break;
	case ECharMoveState::SideStepLeft:
	case ECharMoveState::SideStepRight:
		DesiredAnimation = SideStepAnimation;
		break;
	default:
		DesiredAnimation = IdleAnimation;
		break;
	}

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

void Aggj16_slamjamCharacter::SetMove(FVector Direction, ECharMoveState MoveState)
{
	if (bCanMove)
	{
		prevMoveState = moveState;
		moveState = MoveState;
		facingDirection = MoveState;

		if (!WillHitWall(moveDistance))
		{
			MoveList = MoveQueue.GetActions();
			SetMoveTarget(GetActorLocation() + Direction * moveDistance);
			bStopMoving = false;
			bCanMove = false;
			speed = moveSpeed;
			bIsMoving = true;
		}
		else
		{
			prevMoveState = MoveState;
			moveState = ECharMoveState::Idle;
		}
	}
}

void Aggj16_slamjamCharacter::MoveRight()
{
	SetMove(FVector(1.0, 0.0, 0.0), ECharMoveState::MoveRight);
}

void Aggj16_slamjamCharacter::MoveUp()
{
	SetMove(FVector(0.0, -1.0, 0.0), ECharMoveState::MoveUp);
}

void Aggj16_slamjamCharacter::MoveDown()
{
	SetMove(FVector(0.0, 1.0, 0.0), ECharMoveState::MoveDown);
}

void Aggj16_slamjamCharacter::MoveLeft()
{
	SetMove(FVector(-1.0, 0.0, 0.0), ECharMoveState::MoveLeft);
}

void Aggj16_slamjamCharacter::Jump()
{
	FVector moveDir = GetMoveDirection();
	if (WillHitWall(moveDistance * 2))
	{
		if (WillHitWall(moveDistance))
		{
			MoveIndex = MoveList.Num();
			bStopMoving = false;
		}
		else
		{
			SetMoveTarget(GetActorLocation() + moveDir * moveDistance);
		}
	}
	else
	{
		SetMoveTarget(GetActorLocation() + moveDir * moveDistance * 2);
	}
	speed = jumpSpeed;
}

void Aggj16_slamjamCharacter::Roll()
{
	FVector moveDir = GetMoveDirection();
	if (WillHitWall(moveDistance * 2))
	{
		if (WillHitWall(moveDistance))
		{
			MoveIndex = MoveList.Num();
			bStopMoving = false;
		}
		else
		{
			SetMoveTarget(GetActorLocation() + moveDir * moveDistance);
		}
	}
	else
	{
		SetMoveTarget(GetActorLocation() + moveDir * moveDistance * 2);
	}
	speed = rollSpeed;
}

void Aggj16_slamjamCharacter::SideStepLeft()
{
	FVector moveDir = GetMoveDirection();
	if (WillHitWall(moveDistance))
	{
		MoveIndex = MoveList.Num();
		bStopMoving = false;
	}
	else
	{
		SetMoveTarget(GetActorLocation() + moveDir * moveDistance);
	}
	speed = sidestepSpeed;
}

void Aggj16_slamjamCharacter::SideStepRight()
{
	FVector moveDir = GetMoveDirection();
	if (WillHitWall(moveDistance))
	{
		MoveIndex = MoveList.Num();
		bStopMoving = false;
	}
	else
	{
		SetMoveTarget(GetActorLocation() + moveDir * moveDistance);
	}
	speed = sidestepSpeed;
}

FVector Aggj16_slamjamCharacter::GetMoveDirection()
{
	FVector moveDirection;
	switch (moveState)
	{
	case ECharMoveState::MoveDown:
		moveDirection = FVector(0.0, 1.0, 0.0);
		break;
	case ECharMoveState::MoveUp:
		moveDirection = FVector(0.0, -1.0, 0.0);
		break;
	case ECharMoveState::MoveRight:
		moveDirection = FVector(1.0, 0.0, 0.0);
		break;
	case ECharMoveState::MoveLeft:
		moveDirection = FVector(-1.0, 0.0, 0.0);
		break;
	case ECharMoveState::SideStepLeft:
		moveDirection = GetActorForwardVector();
		break;
	case ECharMoveState::SideStepRight:
		moveDirection = GetActorForwardVector() * -1;
		break;
	case ECharMoveState::Roll:
		moveDirection = GetActorRightVector();
		break;
	case ECharMoveState::Jump:
		moveDirection = GetActorRightVector();
		break;
	default:
		moveDirection = FVector(0.0, 0.0, 0.0);
		break;
	}
	return moveDirection;
}

FVector Aggj16_slamjamCharacter::GetFacingDirection()
{
	FVector moveDirection;
	switch (facingDirection)
	{
	case ECharMoveState::MoveDown:
		moveDirection = FVector(0.0, 1.0, 0.0);
		break;
	case ECharMoveState::MoveUp:
		moveDirection = FVector(0.0, -1.0, 0.0);
		break;
	case ECharMoveState::MoveRight:
		moveDirection = FVector(1.0, 0.0, 0.0);
		break;
	case ECharMoveState::MoveLeft:
		moveDirection = FVector(-1.0, 0.0, 0.0);
		break;
	default:
		moveDirection = FVector(0.0, 0.0, 0.0);
		break;
	}
	return moveDirection;
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

void Aggj16_slamjamCharacter::FinishedMove()
{
	prevMoveState = moveState;
	if (MoveIndex < MoveList.Num() && !bStopMoving)
	{
		moveState = MoveList[MoveIndex];
		MoveIndex++;
		PerformNextMove();
	}
	else 
	{
		MoveIndex = 0;
		moveState = ECharMoveState::Idle;
		bCanMove = true;
		bStopMoving = true;
		bIsMoving = false;
	}
}

void Aggj16_slamjamCharacter::NotifyHit
(
class UPrimitiveComponent * MyComp,
	AActor * Other,
class UPrimitiveComponent * OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult & Hit
	)
{
	GLog->Log("WE shouldn't see this, unless its an interactible");
	SetMoveTarget(prevLocation);
	bStopMoving = true;
	MoveIndex = MoveList.Num();
}

void Aggj16_slamjamCharacter::PerformNextMove()
{
	switch (moveState)
	{
	case ECharMoveState::Jump:
		Jump();
		break;
	case ECharMoveState::SideStepLeft:
		SideStepLeft();
		break;
	case ECharMoveState::SideStepRight:
		SideStepRight();
		break;
	case ECharMoveState::Roll:
		Roll();
		break;
	default:

		break;
	}
}

void Aggj16_slamjamCharacter::MoveLoop(float DeltaSeconds)
{
	if (!bIsMoving)
	{
		return;
	}

	FVector ActorPos = GetActorLocation();

	//FHitResult HitInfo;//the thing that is an output of the statement

	//FCollisionQueryParams Line(FName("Collision param"), true);
	//Line.AddIgnoredActor(this);

	////bool DidHit = GetWorld()->LineTraceSingle(HitInfo, GetActorLocation(), GetActorLocation() + GetMoveDirection() * 100, ECC_PhysicsBody, Line);
	//bool DidHit = GetWorld()->LineTraceSingleByChannel(HitInfo, GetActorLocation(), GetActorLocation() + GetMoveDirection() * 100, ECC_PhysicsBody, Line);
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetMoveDirection() * 100, FColor::Red, false, 1.0f);
	//
	//if (DidHit)
	//{
	//	SetMoveTarget(prevLocation);
	//	MoveIndex = MoveList.Num();
	//}

	if (FVector::PointsAreNear(ActorPos, moveTarget, 5))
	{
		SetActorLocation(moveTarget);
		FinishedMove();
	}
	else
	{
		FVector newPos = FMath::Lerp(ActorPos, moveTarget, DeltaSeconds * speed);
		SetActorLocation(newPos);
	}
}

void Aggj16_slamjamCharacter::SetMoveTarget(FVector newLocation)
{
	prevLocation = moveTarget;
	moveTarget = newLocation;
}

void Aggj16_slamjamCharacter::UpdateCharacter(float DeltaSeconds)
{
	// Update animation to match the motion
	UpdateAnimation();

	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
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

		MoveLoop(DeltaSeconds);
	}
}

ECharMoveState Aggj16_slamjamCharacter::GetMoveState()
{
	return moveState;
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

bool Aggj16_slamjamCharacter::WillHitWall(float Distance)
{
	FVector ActorPos = GetActorLocation();

	FHitResult HitInfo;

	FCollisionQueryParams Line(FName("Collision param"), true);
	Line.AddIgnoredActor(this);

	return GetWorld()->LineTraceSingleByChannel(HitInfo, GetActorLocation(), GetActorLocation() + GetMoveDirection() * Distance, ECC_PhysicsBody, Line);
}
