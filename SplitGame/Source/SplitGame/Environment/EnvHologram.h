// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Environment/EnvObject.h"
#include "Environment/EnvKnobInterface.h"
#include "EnvHologram.generated.h"

class UBoxComponent;

UCLASS()
class SPLITGAME_API AEnvHologram : public AEnvObject, public IEnvKnobInterface
{
	GENERATED_BODY()

	virtual bool KnobActivated() override;

	virtual bool KnobDeactivated() override;
	
	FVector mDefaultExtent = FVector(40.0f, 40.f, 10.f);

	UMaterialInstanceDynamic* mDynamicMaterialInstance;

	UBoxComponent* mBoxComponent;
		   
public:
	AEnvHologram();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* mMeshComponent;
			   
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector mExtent = mDefaultExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool mIsVisible = false;

	void Materialize();
	void Unmaterialize();

	virtual void OnConstruction(const FTransform & Transform);

	virtual void Refresh();

	class USFXComponent* mSFXAppear;
	class USFXComponent* mSFXDisappear;
};
