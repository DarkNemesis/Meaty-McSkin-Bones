// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/AnimTask/AnimTask.h"
#include "AnimTask_MoveToLocation.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API UAnimTask_MoveToLocation : public UAnimTask
{
	GENERATED_BODY()
	
public:
	void Init(FVector InLocation) { Location = InLocation; }

protected:
	virtual void OnTaskTick() override;

private:
	FVector Location;
	
	
};
