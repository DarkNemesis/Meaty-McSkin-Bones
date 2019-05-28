// Fill out your copyright notice in the Description page of Project Settings.

#include "SkeletonCharacterMovement.h"
#include "MuscleCharacter.h"
#include "SplitGame/SkeletonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Component/AnimationAdjustmentComponent.h"
#include "Animations/AnimTask/AnimTask_MoveToLocation.h"
#include "Animations/AnimTask/AnimTask_TurnTo.h"
#include "Animations/AnimTask/AnimTask_StartClimbOnMuscle.h"
#include "Misc/OutputDeviceNull.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"


void USkeletonCharacterMovement::ClimbToJump()
{
		Velocity.Z = FMath::Max(Velocity.Z, JumpZVelocity * mJumpSpeedMultiplier);
}

bool USkeletonCharacterMovement::CheckJump()
{
	if (mOwner->bPressedJump)
	{
		if (MovementMode == MOVE_Custom)
		{
			switch (CustomMovementMode)
			{
			case (uint8)ESkeletonCustomMoveMode::Climb:
			{
				ClimbToJump();
				break;
			}
			case (uint8)ESkeletonCustomMoveMode::OnMuscle:
			{
				if (mOwner->GetAnimationType() == ESkeletonCustomAnimation::ClimbOnMuscle)
					return false;
				Velocity.Z = JumpZVelocity;
				break;
			}
				default:
				break;
			}

			SetMovementMode(MOVE_Falling, 0);
			mOwner->SetAnimationType(ESkeletonCustomAnimation::None);
			mOwner->bPressedJump = false;
			mOwner->JumpCurrentCount = 1;
			mOwner->OnJumped();
			mOwner->bWasJumping = true;
			return true;

		}
	}

	return false;
}

void USkeletonCharacterMovement::ClimbOnMuscle(AMuscleCharacter * inMuscleCharacter)
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ESkeletonCustomMoveMode::OnMuscle)
		return;

	float topOffset = -50;
	FVector topLocation =  inMuscleCharacter->GetActorLocation() + FVector::UpVector * (inMuscleCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() +
		mOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + topOffset);
	// rotate muscle 180 degrees
	if (inMuscleCharacter->GetActorForwardVector().Y * mOwner->GetActorForwardVector().Y > 0)
	{
		inMuscleCharacter->SetActorRotation(FRotator(0, -mOwner->GetActorRotation().Yaw, 0));
	}

	FVector direction = FVector(0, inMuscleCharacter->GetActorForwardVector().Y >= 0 ? -1 : 1, 0);
	float offset = 40;
	FVector startLocation = inMuscleCharacter->GetActorLocation() + direction * offset;
	mpMuscle = inMuscleCharacter;
	FCollisionQueryParams params;
	params.AddIgnoredActor(mOwner);
	params.AddIgnoredActor(inMuscleCharacter);

	//DrawDebugCapsule(GetWorld(), startLocation, mOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 10,
		//mOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() + 10, mOwner->GetActorQuat(), FColor::Red, false, 2.0f);

	float inflation = 10;
	UWorld* world = GetWorld();
	if (world)
	{
		if (!world->OverlapBlockingTestByChannel(topLocation, mOwner->GetActorQuat(), ECC_Pawn, mOwner->GetCapsuleComponent()->GetCollisionShape(), params)
			&& !world->SweepTestByChannel(mOwner->GetActorLocation(), startLocation, mOwner->GetActorQuat(), ECC_Pawn, 
				mOwner->GetCapsuleComponent()->GetCollisionShape(-0.1f), params)
			&& world->OverlapBlockingTestByChannel(startLocation, mOwner->GetActorQuat(), ECC_Pawn, mOwner->GetCapsuleComponent()->GetCollisionShape(inflation), params))
		{
			// play some animation to climb to the top of muscle
			UAnimationAdjustmentComponent* adjustment = (UAnimationAdjustmentComponent*)(mOwner->GetComponentByClass(UAnimationAdjustmentComponent::StaticClass()));
			check(adjustment);
			if (adjustment->GetTasks().Num() != 0)
				return;

			auto moveToLocation = NewObject<UAnimTask_MoveToLocation>();
			moveToLocation->SetOwner(mOwner);
			moveToLocation->Init(startLocation);
			adjustment->AddTask(moveToLocation);

			auto turnTo = NewObject<UAnimTask_TurnTo>();
			turnTo->SetOwner(mOwner);
			turnTo->Init(FRotator(0, direction.Y > 0 ? 90 : 270, 0));
			adjustment->AddTask(turnTo);

			auto climb = NewObject<UAnimTask_StartClimbOnMuscle>();
			climb->SetOwner(mOwner);
			adjustment->AddTask(climb);

		}
	}
}

void USkeletonCharacterMovement::ClimbOffMuscle()
{
	if (!(MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ESkeletonCustomMoveMode::OnMuscle) 
		|| mOwner->GetAnimationType() == ESkeletonCustomAnimation::ClimbOnMuscle)
		return;

	bool bIsLeft = mOwner->GetActorForwardVector().Y >= 0;
	FVector topLocation = mOwner->GetActorLocation() + (bIsLeft ? 1 : -1) * FVector::RightVector * (mOwner->GetCapsuleComponent()->GetScaledCapsuleRadius());
	FVector bottomLocation = topLocation - FVector::UpVector * mOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FCollisionQueryParams params;
	params.AddIgnoredActor(mOwner);

	UWorld* world = GetWorld();
	if (world)
	{
		if (!world->OverlapBlockingTestByChannel(topLocation, mOwner->GetActorQuat(), ECC_Pawn, mOwner->GetCapsuleComponent()->GetCollisionShape(-0.1f), params)
			&& !world->OverlapBlockingTestByChannel(bottomLocation, mOwner->GetActorQuat(), ECC_Pawn, mOwner->GetCapsuleComponent()->GetCollisionShape(-0.1f), params))
		{
			// todo: should play some animation to climb to the top of muscle
			mOwner->SetActorLocation(bottomLocation);
			SetMovementMode(MOVE_Falling, 0);
			FOutputDeviceNull ar;
			mOwner->CallFunctionByNameWithArguments(TEXT("EndFix"), ar, NULL, true);
			//mOwner->SetAnimationType(ESkeletonCustomAnimation::ClimbOnMuscle);
		}
	}

}

void USkeletonCharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	mOwner = Cast<ASkeletonCharacter>(GetOwner());

}

void USkeletonCharacterMovement::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	// Attach to muscle
	if (MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ESkeletonCustomMoveMode::OnMuscle && mpMuscle)
	{
		//FAttachmentTransformRules rules(FAttachmentTransformRules::KeepWorldTransform);
		//mOwner->AttachToActor(mpMuscle, rules);
		mpMuscle->bIsCarrySkeleton = true;
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ESkeletonCustomMoveMode::OnMuscle && mpMuscle)
	{
		//FDetachmentTransformRules rules(FDetachmentTransformRules::KeepWorldTransform);
		//mOwner->DetachFromActor(rules);
		mpMuscle->bIsCarrySkeleton = false;
	}
}

void USkeletonCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	//UE_LOG(LogTemp, Log, TEXT("Ticking physcustom"));

	switch (CustomMovementMode)
	{
	case (uint8)ESkeletonCustomMoveMode::Climb:
		ProcessClimb(deltaTime);
		break;
	case (uint8)ESkeletonCustomMoveMode::OnMuscle:
		ProcessOnMuscle(deltaTime);
		break;
	default:
		break;
	}
}

void USkeletonCharacterMovement::ProcessClimb(float DeltaTime)
{
	//UE_LOG(LogTemp, Log, TEXT("Ticking process climb"));

	UWorld* world = GetWorld();
	check(world);

	Velocity = FVector::UpVector * mUpInput *mMaxClimbSpeed;
	ApplyRootMotionToVelocity(DeltaTime);

	if (mOwner->GetAnimationType() == ESkeletonCustomAnimation::None)
	{
		FHitResult result;
		if (!SafeMoveUpdatedComponent(Velocity * DeltaTime, mOwner->GetActorQuat(), true, result, ETeleportType::None))
		{
//			UE_LOG(LogTemp, Log, TEXT("Blocked by %s in the ending of climbing!"), *result.Actor->GetName());
		}
		
		float height = 50;
		float radius = mOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() * 4;
		//float halfHeight = mOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector start = GetActorLocation() + mOwner->GetActorUpVector() * (mOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - height / 2);
		FVector end = start - mOwner->GetActorForwardVector() * radius;
		//FVector bottomStart = start - FVector::UpVector * halfHeight;
		//FVector bottomEnd = end - FVector::UpVector * halfHeight;
		FCollisionShape shape;
		shape.SetBox(FVector(radius, radius, height / 2));
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(mOwner);
		// detect the end of climbing
		if (!world->SweepSingleByChannel(result, start, end, mOwner->GetActorQuat(), ECC_Pawn, shape, queryParams))
			//&& world->SweepSingleByChannel(result, bottomStart, bottomEnd, mOwner->GetActorQuat(), ECC_Pawn, shape, queryParams))
		{
			mOwner->SetAnimationType(ESkeletonCustomAnimation::ClimbUpEnd);
			mOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		
		// check if player is on ground
		float onGroundHeight = 2;
		FVector foot = mOwner->GetActorLocation() - mOwner->GetActorUpVector() * mOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		if (world->LineTraceTestByChannel(foot, foot - mOwner->GetActorUpVector() * onGroundHeight, ECC_Pawn, queryParams))
		{
			mOwner->StopClimb();
		}
	}
	else
	{
		mOwner->AddActorWorldOffset(Velocity* DeltaTime);
	}
}

void USkeletonCharacterMovement::ProcessOnMuscle(float DeltaTime)
{
	Velocity = FVector::ZeroVector;
	//Velocity.Y = mpMuscle->GetVelocity().Y;
	ApplyRootMotionToVelocity(DeltaTime);
	mOwner->AddActorLocalOffset(Velocity * DeltaTime);
}

void USkeletonCharacterMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (mOwner)
	{
		if (mOwner->IsDead())
		{
			SetMovementMode(MOVE_Custom, (uint8)ESkeletonCustomMoveMode::Dead);
			return;
		}
		
		if (!CheckJump())
		{
			if (MovementMode == MOVE_Custom)
			{
				switch (CustomMovementMode)
				{
				case (uint8)ESkeletonCustomMoveMode::Climb:
				{
					ConsumeInputVector();
					//Velocity = FVector::UpVector * mUpInput *mMaxClimbSpeed;
					break;
				}
				case (uint8)ESkeletonCustomMoveMode::OnMuscle:
				{
					ConsumeInputVector();
					break;
				}
				default:
					break;
				}
			}

		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}



