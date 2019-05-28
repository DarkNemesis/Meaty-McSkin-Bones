// Fill out your copyright notice in the Description page of Project Settings.

#include "SplitCameraComponent.h"
#include "SplitGameCharacter.h"
#include "UI/SettingsMenuWidget.h"
#include "Level/LevelManager.h"

// Sets default values for this component's properties
USplitCameraComponent::USplitCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	isCameraEnabled = true;
	mCharacter = (ASplitGameCharacter*)GetOwner();
	mCurrentPeekOffset = FVector(0, 0, 0);	
	mCharacterOffset = FVector(0, 0, 80);

	SetWorldRotation(FRotator(0, 0, 0));
}


// Called when the game starts
void USplitCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}


void USplitCameraComponent::SetEnableCamera(bool i_enable)
{
	isCameraEnabled = i_enable;
}

// Called every frame
void USplitCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto cameraLocation = GetComponentLocation();
	auto targetLocation = GetBaseLocation() + mCurrentPeekOffset;

	FVector cameraBounds = FVector(9000, 9000, 9000);
	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if (levelManager)
	{
		cameraBounds = levelManager->mLevelBounds;
	}

	cameraLocation.Y = FMath::Min(cameraBounds.Y, FMath::Max(0.f, targetLocation.Y));
	cameraLocation.Z = FMath::Min(cameraBounds.Z, FMath::Max(0.f, targetLocation.Z));

	if(isCameraEnabled)
		SetWorldLocation(cameraLocation);
}

void USplitCameraComponent::PeekLeftRight(float Value)
{
	auto preOffset = GetComponentLocation() - GetBaseLocation();
	mCurrentPeekOffset += FVector(0, Peek(Value, preOffset.Y), 0);
}

void USplitCameraComponent::PeekUpDown(float Value)
{
	Value = USettingsMenuWidget::AreControlsInverted() ? -Value : Value;
	
	auto preOffset = GetComponentLocation() - GetBaseLocation();
	mCurrentPeekOffset += FVector(0, 0, Peek(Value, preOffset.Z));
}

float USplitCameraComponent::Peek(float Value, float preOffset)
{
	float z = 0;

	if (preOffset <= -mPeekLimit && Value < 0)
		z = 0;
	else if (preOffset >= mPeekLimit && Value > 0)
		z = 0;
	else if (Value == 0)
	{
		if (-mPeekBackSpeed < preOffset && preOffset < 0)
			z = -preOffset;
		else if (0 < preOffset && preOffset < mPeekBackSpeed)
			z = -preOffset;
		else if (preOffset == 0)
			z = 0;
		else
			z = FMath::Sign(preOffset) * -mPeekBackSpeed;
	}
	else
		z = (Value * mPeekSpeed);

	return z;
}

FVector USplitCameraComponent::GetBaseLocation()
{
	return mCharacter->GetActorLocation() + mCharacterOffset;
}

