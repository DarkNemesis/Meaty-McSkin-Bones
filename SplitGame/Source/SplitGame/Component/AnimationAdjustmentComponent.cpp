// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimationAdjustmentComponent.h"

#include "SplitGameCharacter.h"
#include "Classes/Curves/CurveVector.h"
#include "Classes/Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UAnimationAdjustmentComponent::UAnimationAdjustmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAnimationAdjustmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UAnimationAdjustmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// remove finished task
	if (AnimTasks.Num() >= 1)
	{
		if (AnimTasks[0]->IsFinished())
			AnimTasks.RemoveAt(0);
	}

	// run first task
	if (AnimTasks.Num() >= 1)
	{
		AnimTasks[0]->TickTask();
	}

	// apply root curve
	if (CurrentRootCurve != ERootCurveType::None)
	{
		auto curves = RootCurveMap[CurrentRootCurve];

		auto locationCurve = curves.LocationCurve;
		if (locationCurve)
		{
			FVector value = locationCurve->GetVectorValue(GetOwner()->GetGameTimeSinceCreation() - mCurveStartTime);
			value.Y = bIsLeft ? -value.Y : value.Y;
			FVector offset = value * 100 - (GetOwner()->GetActorLocation() - mCurveStartLocation);
			offset += DeltaTime * mAddtiveLocationSpeed;
			GetOwner()->AddActorWorldOffset(offset);
		}

		auto rotationCurve = curves.RotationCurve;
		if (rotationCurve)
		{
			FVector value = rotationCurve->GetVectorValue(GetOwner()->GetGameTimeSinceCreation() - mCurveStartTime);
			FVector vector = (value * 180);
			FRotator rotator = FRotator(vector.X, vector.Y, vector.Z) - (GetOwner()->GetActorRotation() - mCurveStartRotator);
			GetOwner()->AddActorWorldRotation(rotator);

		}

	}

}

void UAnimationAdjustmentComponent::StartRootCurveAdjust(ERootCurveType inCurveType, FVector inAddtiveLocationSpeed)
{
	mCurveStartTime = GetOwner()->GetGameTimeSinceCreation();
	CurrentRootCurve = inCurveType;
	mCurveStartLocation = GetOwner()->GetActorLocation();
	mCurveStartRotator = GetOwner()->GetActorRotation();
	bIsLeft = GetOwner()->GetActorForwardVector().Y <= 0 ? false : true;
	mAddtiveLocationSpeed = inAddtiveLocationSpeed;
}

void UAnimationAdjustmentComponent::EndRootCurveAdjust(ERootCurveType inCurveType)
{
	if (CurrentRootCurve == inCurveType)
	{
		CurrentRootCurve = ERootCurveType::None;
		mCurveStartTime = 0;
		mCurveStartLocation = FVector::ZeroVector;
		mCurveStartRotator = FRotator::ZeroRotator;
	}
}

void UAnimationAdjustmentComponent::SetOffsetSpeed(FVector inOffsetSpeed)
{
}

