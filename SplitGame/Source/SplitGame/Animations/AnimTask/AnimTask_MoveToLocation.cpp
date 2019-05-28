// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimTask_MoveToLocation.h"
#include "GameFramework/CharacterMovementComponent.h"

#define RADIUS_THRESHOLD 5

void UAnimTask_MoveToLocation::OnTaskTick()
{
	FVector direction = (Location - Owner->GetActorLocation()).GetSafeNormal();
	FVector diff = Location - Owner->GetActorLocation();
	if (FMath::Abs(diff.Y) <= RADIUS_THRESHOLD)
	{
		Owner->AddActorWorldOffset(diff);
		FVector actorlocation = Owner->GetActorLocation();
		Owner->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		EndTask();
	}
	else
	{
		Owner->AddMovementInput(direction);
	}
}


