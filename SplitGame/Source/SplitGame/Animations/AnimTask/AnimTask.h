// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SplitGameCharacter.h"
#include "AnimTask.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API UAnimTask : public UObject
{
	GENERATED_BODY()

public:
	void SetOwner(ASplitGameCharacter* owner) { Owner = owner; }
	void StartTask() { OnTaskStart(); }
	void EndTask() { bIsFinished = true; OnTaskEnd(); }
	void TickTask() { OnTaskTick(); }
	bool IsFinished() { return bIsFinished; }

protected:
	virtual void OnTaskStart() {}
	virtual void OnTaskTick() {}
	virtual void OnTaskEnd() {}
	
protected:
	ASplitGameCharacter* Owner;
	bool bIsFinished = false;
	
};
