// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/AnimTask/AnimTask.h"
#include "AnimTask_StartClimbOnMuscle.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API UAnimTask_StartClimbOnMuscle : public UAnimTask
{
	GENERATED_BODY()
	

protected:
	virtual void OnTaskTick() override;

	
};
