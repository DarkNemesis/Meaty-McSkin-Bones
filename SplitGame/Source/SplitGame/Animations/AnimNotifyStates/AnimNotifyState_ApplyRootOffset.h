// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "Component/AnimationAdjustmentComponent.h"
#include "AnimNotifyState_ApplyRootOffset.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API UAnimNotifyState_ApplyRootOffset : public UAnimNotifyState
{
	GENERATED_BODY()

	
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curve")
		ERootCurveType Type;
};
