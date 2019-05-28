// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Environment/EnvObject.h"
#include "Environment/EnvKnobInterface.h"
#include <Runtime/Engine/Classes/Engine/EngineTypes.h>
#include <Runtime/Engine/Public/TimerManager.h>

#include "EnvElevator.generated.h"

UENUM(Meta = (Bitflags))
enum class EElevatorDoorBits : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

/**
 * 
 */
UCLASS()
class SPLITGAME_API AEnvElevator : public AEnvObject, public IEnvKnobInterface
{
	GENERATED_BODY()


public:
	AEnvElevator();

	UFUNCTION(BlueprintCallable)
	virtual bool KnobActivated() override;

	virtual void Refresh();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* MeshComponent;

private:
	UFUNCTION()
		void Move();
	UFUNCTION()
		void MoveDoor(bool bBottom, bool bOpen);

	void ActivateDoor(bool bBottom, bool bOpen);


	UPROPERTY(EditAnywhere, Category = "Env Elevator", meta = (DisplayName = "Speed", AllowPrivateAccess = "true"))
		float mSpeed;
	UPROPERTY(EditAnywhere, Category = "Env Elevator", Meta = (Bitmask, BitmaskEnum = "EElevatorDoorBits"), meta = (DisplayName = "Door Support", AllowPrivateAccess = "true"))
		uint8 mDoorSupport = mDefaultDoorSupport;
	UPROPERTY(EditAnywhere, Category = "Env Elevator", meta = (DisplayName = "Door Frame Mesh", AllowPrivateAccess = "true"))
		UStaticMesh* mDoorFrameMesh;
	UPROPERTY(EditAnywhere, Category = "Env Elevator", meta = (DisplayName = "Door Mesh", AllowPrivateAccess = "true"))
		UStaticMesh* mDoorMesh;
	UPROPERTY(EditAnywhere, Category = "Env Elevator", meta = (DisplayName = "Extend", AllowPrivateAccess = "true"))
		FVector mExtend = mDefaultExtend;
	UPROPERTY(EditAnywhere, Category = "Env Elevator", meta = (DisplayName = "Door Width", AllowPrivateAccess = "true"))
		float mDoorWidth = 500;
	UPROPERTY(EditAnywhere, Category = "Env Elevator", meta = (DisplayName = "Start From Bottom", AllowPrivateAccess = "true"))
		bool bStartFromBottom = true;

	UPROPERTY()
	TArray<UStaticMeshComponent*> mDoorMeshComponents;
	UPROPERTY()
	TArray<UStaticMeshComponent*> mDoorFrameComponents;
	UPROPERTY()
	TArray<AActor*> mAttachedActors;

	class USFXComponent* mActiveAudio;

	FTimerHandle mTimerHandle;
	FTimerDelegate mTimerDelegate;
	FTimerHandle mDoorTimeHandle;
	FTimerDelegate mDoorDelegate;
	FVector mDirection;
	FVector mDefaultExtend = FVector(1000, 1000, 5000);
	FVector mOffset;

	uint8 mDefaultDoorSupport = 0xF;
};
