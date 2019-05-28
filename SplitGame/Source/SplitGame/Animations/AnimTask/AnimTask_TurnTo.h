// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/AnimTask/AnimTask.h"
#include "AnimTask_TurnTo.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API UAnimTask_TurnTo : public UAnimTask
{
	GENERATED_BODY()
	
public:
	void Init(FRotator rotation) { TargetRotation = rotation;  Direction = rotation.Yaw >= 0 ? -1 : 1; };

protected:
	virtual void OnTaskTick() override;

private:
	FRotator TargetRotation;
	float Direction;
	float Speed = 10;
	
};
