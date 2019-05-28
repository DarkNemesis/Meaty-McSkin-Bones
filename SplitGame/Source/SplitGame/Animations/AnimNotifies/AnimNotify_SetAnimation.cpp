// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SetAnimation.h"
//#include "SkeletonCharacter.h"
#include "Components/SkeletalMeshComponent.h"


UAnimNotify_SetAnimation::UAnimNotify_SetAnimation() 
	: Super()
{
}

#if WITH_EDITOR
void UAnimNotify_SetAnimation::PostLoad()
{
	Super::PostLoad();
}

void UAnimNotify_SetAnimation::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

FString UAnimNotify_SetAnimation::GetNotifyName_Implementation() const
{
	return FString(TEXT("Set %d"), static_cast<uint8>(mNewAnimation));
}

void UAnimNotify_SetAnimation::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	ASkeletonCharacter* character = Cast<ASkeletonCharacter>(MeshComp->GetOwner());
	if (character != nullptr)
	{
		character->SetAnimationType(mNewAnimation);
	}
}

