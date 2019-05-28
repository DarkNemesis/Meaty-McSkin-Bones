// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimTask_StartClimbOnMuscle.h"
#include "Components/CapsuleComponent.h"
#include "SkeletonCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimTask_StartClimbOnMuscle::OnTaskTick()
{
	if (Owner)
	{
		auto SkeletonCharacter = Cast<ASkeletonCharacter>(Owner);
		if (SkeletonCharacter)
		{
			SkeletonCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SkeletonCharacter->GetCapsuleComponent()->SetEnableGravity(false);
			SkeletonCharacter->SetAnimationType(ESkeletonCustomAnimation::ClimbOnMuscle);
			SkeletonCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Custom, (uint8)ESkeletonCustomMoveMode::OnMuscle);
		}
	}
	EndTask();
 }



