// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/PrimitiveComponent.h"
#include "AnimNotify_SetCollision.generated.h"

UCLASS(const, hidecategories = Object, EditInlineNew, classGroup = "CharacterNotify", meta = (DisplayName = "Set Collision"))
class SPLITGAME_API UAnimNotify_SetCollision : public UAnimNotify
{
	GENERATED_BODY()
	
	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	// End UAnimNotify interface

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "Collision Type"))
		TEnumAsByte<ECollisionEnabled::Type> mCollisionType;
	
};
