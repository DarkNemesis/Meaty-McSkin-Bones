// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/EngineTypes.h"
#include "AnimNotify_SetMovementMode.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Set Movement Mode"))
class SPLITGAME_API UAnimNotify_SetMovementMode : public UAnimNotify
{
	GENERATED_BODY()
	
	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	// End UAnimNotify interface

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "New Movement Mode"))
		TEnumAsByte<EMovementMode> mMovementMode;
	
};
