// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Classes/Curves/CurveVector.h"
#include "Animations/AnimTask/AnimTask.h"
#include "AnimationAdjustmentComponent.generated.h"

UENUM(BlueprintType)
enum class ERootCurveType : uint8
{
	None,
	ClimbOntoLedge,
	ClimbDownLedge,
	ClimbOntoMuscle
};

USTRUCT(BlueprintType)
struct FRootCurves
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCurveVector* LocationCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCurveVector* RotationCurve;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPLITGAME_API UAnimationAdjustmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimationAdjustmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddTask(UAnimTask* ipTask) { AnimTasks.Add(ipTask); }
	TArray<UAnimTask*>& GetTasks() { return AnimTasks; }


	UFUNCTION(BlueprintCallable, Category = "Animation")
		void StartRootCurveAdjust(ERootCurveType inCurveType, FVector inAddtiveLocationSpeed = FVector::ZeroVector);
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void EndRootCurveAdjust(ERootCurveType inCurveType);
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void SetOffsetSpeed(FVector inOffsetSpeed);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animation")
	TMap<ERootCurveType, FRootCurves> RootCurveMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		ERootCurveType CurrentRootCurve;

private:
	UPROPERTY()
		TArray<UAnimTask*> AnimTasks;

	FVector mCurveStartLocation;
	FRotator mCurveStartRotator;
	FVector mAddtiveLocationSpeed;
	float mCurveStartTime;
	bool bIsLeft;
};
