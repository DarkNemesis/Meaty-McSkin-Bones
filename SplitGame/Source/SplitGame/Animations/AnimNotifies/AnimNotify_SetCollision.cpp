// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SetCollision.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

FString UAnimNotify_SetCollision::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_SetCollision::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());
	if (character != nullptr)
	{
		character->GetCapsuleComponent()->SetCollisionEnabled(mCollisionType);
	}
}
