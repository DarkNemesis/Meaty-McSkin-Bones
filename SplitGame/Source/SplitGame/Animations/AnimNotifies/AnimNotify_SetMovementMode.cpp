// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SetMovementMode.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

FString UAnimNotify_SetMovementMode::GetNotifyName_Implementation() const
{
	return FString("Set movement mode to");
}

void UAnimNotify_SetMovementMode::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());
	if (character != nullptr)
	{
		character->GetCharacterMovement()->SetMovementMode(mMovementMode);
	}
}
