// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetRotation.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, editinlinenew, classGroup = "CharacterNotify", meta = (DisplayName = "Set Rotation"))
class SPLITGAME_API UAnimNotify_SetRotation : public UAnimNotify
{
	GENERATED_BODY()
	
	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	// End UAnimNotify interface

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "New Rotation"))
	FRotator mNewRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		bool bFrozenX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		bool bFrozenY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		bool bFrozenZ;

};
