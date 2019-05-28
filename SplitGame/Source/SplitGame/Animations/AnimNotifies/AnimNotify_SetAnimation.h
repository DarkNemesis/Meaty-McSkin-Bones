// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SkeletonCharacter.h"
#include "AnimNotify_SetAnimation.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, classGroup = "CharacterNotify", meta = (DisplayName = "Set SkeletonCustomAnimation"))
class SPLITGAME_API UAnimNotify_SetAnimation : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UAnimNotify_SetAnimation();
#if WITH_EDITOR
	// Begin UObject interface
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// End UObject interface

	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	// End UAnimNotify interface

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "New Animation Type"))
	ESkeletonCustomAnimation mNewAnimation;
	
};
