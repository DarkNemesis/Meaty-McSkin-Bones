// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimTask_TurnTo.h"
#include "Engine/World.h"

#define ANGLE_THRESHOLD 10


void UAnimTask_TurnTo::OnTaskTick()
{
	if (Owner != nullptr)
	{
		if (UWorld* world = Owner->GetWorld())
		{

			FQuat deltaQuat(FVector(0, 0, 1), Direction * Speed * world->GetDeltaSeconds());
			Owner->AddActorWorldRotation(deltaQuat);
			auto rotation = Owner->GetActorRotation();
			float distance = (int)(Owner->GetActorRotation().Yaw - TargetRotation.Yaw) % 360;
			if (FMath::Abs(distance) <= ANGLE_THRESHOLD)
			{
				Owner->AddActorWorldRotation(FRotator(0, distance, 0));
				EndTask();
			}
		}

	}
	
}



