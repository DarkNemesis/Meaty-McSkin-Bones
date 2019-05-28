// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkinCatapultComponent.generated.h"

class ASkeletonCharacter;
class ASkinCharacter;
class UParticleSystemComponent;
class UParticleSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLITGAME_API USkinCatapultComponent : public UActorComponent
{
	GENERATED_BODY()

	FVector mPointOfOrigin;
	FVector mMaxPointOfAim;
	FVector mMinPointOfAim;

	ASkinCharacter*		mSkinCharacter;
	ASkeletonCharacter* mSkeletonCharacter;
	UParticleSystemComponent* mProjectileTrajectory;

	UParticleSystem* mProjectileParticle;
	TArray<UParticleSystemComponent*> BeamArray;

	int mDirection;
	const float mMaxAimableDistance = 400;
	const float mMinAimableDistance = 125;
	const float mLaunchVelocity = 2500.f;

public:	
	USkinCatapultComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetParameters(ASkinCharacter* Skin, ASkeletonCharacter* Skeleton);	

	void KeepInBounds();

	void Launch();

	void DrawProjectileTrajectory();

	FVector GetProjectileDirection();

	void DestroyComponent(bool bPromoteChildren = false) override;
};
