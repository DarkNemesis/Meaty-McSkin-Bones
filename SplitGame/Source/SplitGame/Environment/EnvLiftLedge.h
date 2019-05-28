// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvObject.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "EnvLiftLedge.generated.h"

UCLASS()
class SPLITGAME_API AEnvLiftLedge : public AEnvObject
{
	GENERATED_BODY()

	UBoxComponent* mBoxComponent;

	FVector mDefaultExtent = FVector(40.0f, 40.f, 40.f);
	FVector mDefaultHandle = FVector(40.0f, 40.f, 40.f);

	float mTriggerExtent = 80.f;

	float mInitialZ = 0;

	bool bIsBeingHeld = false;
	
public:	
	// Sets default values for this actor's properties
	AEnvLiftLedge();

	void OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void Action(ASplitGameCharacter* Character);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector mExtent = mDefaultExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector mHandle = mDefaultHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mDestinationZ = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mRiseSpeed = 7.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mFallSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* mMeshComponent;

	void Refresh();

	virtual void Tick(float DeltaTime) override;
};
