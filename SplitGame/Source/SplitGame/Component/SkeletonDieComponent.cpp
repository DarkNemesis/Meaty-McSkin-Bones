// Fill out your copyright notice in the Description page of Project Settings.

#include "SkeletonDieComponent.h"
#include "GameFramework/Actor.h"
//#include "SplitGame/SplitGameCharacter.h"
#include "SkeletonCharacter.h"

// Sets default values for this component's properties
USkeletonDieComponent::USkeletonDieComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkeletonDieComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void USkeletonDieComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* owner = GetOwner();
	check(owner);

	if (bIsRecording)
	{
		float offset = OldLocationZ - owner->GetActorLocation().Z;
		if (offset > 0)
			CurrentFallingDistance += offset;
		OldLocationZ = owner->GetActorLocation().Z;
		if (CurrentFallingDistance >= MaxFallingDistance)
			IsAboveDangerDistance = true;
	}
}

void USkeletonDieComponent::StartRecord()
{
	AActor* owner = GetOwner();
	check(owner);

	if (!bIsRecording)
	{
		bIsRecording = true;
		OldLocationZ = owner->GetActorLocation().Z;
		CurrentFallingDistance = 0;
	}
	IsAboveDangerDistance = false;
}

void USkeletonDieComponent::EndRecord()
{
	AActor* owner = GetOwner();
	check(owner);


	bIsRecording = false;
	if (CurrentFallingDistance >= MaxFallingDistance)
	{
		ASkeletonCharacter* character = Cast<ASkeletonCharacter>(owner);
		if (character != nullptr && !character->b_IsCharacterFloating)
		{
			character->SetDead(true);
			//owner->Destroy();
			return;
		}
	}
	IsAboveDangerDistance = false;
}

void USkeletonDieComponent::Reset()
{
	bIsRecording = false;
	CurrentFallingDistance = 0;
	IsAboveDangerDistance = false;
}

