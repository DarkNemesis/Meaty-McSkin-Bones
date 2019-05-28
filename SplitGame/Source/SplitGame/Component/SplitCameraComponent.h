// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "SplitCameraComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLITGAME_API USplitCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	float mPeekSpeed = 4;
	float mPeekLimit = 300;
	float mPeekBackSpeed = 10;

	//This stores the camera offset when the character is peeking
	FVector mCurrentPeekOffset;

	//This stores the default camera offset that every character has from start
	FVector mCharacterOffset;
	
	class ASplitGameCharacter* mCharacter;

public:	
	// Sets default values for this component's properties
	USplitCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool isCameraEnabled;

public:	

	void SetEnableCamera(bool i_enable);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PeekLeftRight(float Value);

	void PeekUpDown(float Value);

	float Peek(float Value, float preOffset);

	FVector GetBaseLocation();
};
