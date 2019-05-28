// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SkeletonCharacterMovement.generated.h"

/**
 *	Skeleton Character Movement is the specific movement for Skeleton Character
 */
UCLASS()
class SPLITGAME_API USkeletonCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USkeletonCharacterMovement() {};
	virtual ~USkeletonCharacterMovement() override {}

	void SetUpInput(float inUp) { mUpInput = inUp; }
	UFUNCTION(BlueprintPure, Category = "Skeleton Character Movement")
	float GetUpInput() { return mUpInput; }

	bool CheckJump();
	void ClimbOnMuscle(class AMuscleCharacter* inMuscleCharacter);
	void ClimbOffMuscle();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 previousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skeleton Movement: Climb", meta = (AllowPrivateAccess = "true", DisplayName = "Max Climb Speed"))
		float mMaxClimbSpeed = 200;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skeleton Movement", meta = (AllowPrivateAccess = "true", ClampMax = "1.0", ClampMin = "0.0", DisplayName = "Jump Speed Multiplier"))
		float mJumpSpeedMultiplier = 0.5;
	
	void ClimbToJump();
	void ProcessClimb(float DeltaTime);
	void ProcessOnMuscle(float DeltaTime);

	float mUpInput;
	FVector mOnMuscleLocation;
	class ASkeletonCharacter* mOwner;
	class AMuscleCharacter* mpMuscle;

};
