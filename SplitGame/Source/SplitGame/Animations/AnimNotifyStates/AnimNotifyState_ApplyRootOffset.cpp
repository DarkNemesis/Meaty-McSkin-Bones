// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_ApplyRootOffset.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_ApplyRootOffset::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Received_NotifyBegin(MeshComp, Animation, TotalDuration);
	auto owner = MeshComp->GetOwner();
	if (owner)
	{
		auto adjustment = Cast<UAnimationAdjustmentComponent>(owner->GetComponentByClass(UAnimationAdjustmentComponent::StaticClass()));
		if (adjustment)
			adjustment->StartRootCurveAdjust(Type);
	}
	
}

void UAnimNotifyState_ApplyRootOffset::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Received_NotifyEnd(MeshComp, Animation);
	auto owner = MeshComp->GetOwner();
	if (owner)
	{
		auto adjustment = Cast<UAnimationAdjustmentComponent>(owner->GetComponentByClass(UAnimationAdjustmentComponent::StaticClass()));
		if (adjustment)
			adjustment->EndRootCurveAdjust(Type);
	}
}
