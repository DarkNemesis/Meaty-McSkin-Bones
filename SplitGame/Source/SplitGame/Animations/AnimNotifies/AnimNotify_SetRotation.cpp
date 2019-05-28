// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SetRotation.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFrameWork/Actor.h"

FString UAnimNotify_SetRotation::GetNotifyName_Implementation() const
{
	//return FString();
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_SetRotation::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	AActor* actor = MeshComp->GetOwner();
	if (actor)
	{
		float roll = mNewRotation.Roll;
		float yaw = mNewRotation.Yaw;
		float pitch = mNewRotation.Pitch;
		FRotator oldRotator = actor->GetActorRotation();
		if (bFrozenX)
		{
			roll = oldRotator.Roll;
		}
		if (bFrozenY)
		{
			pitch = oldRotator.Pitch;
		}
		if (bFrozenZ)
		{
			yaw = oldRotator.Yaw;
		}

		actor->SetActorRotation(FRotator(pitch, yaw, roll));

	}
}
