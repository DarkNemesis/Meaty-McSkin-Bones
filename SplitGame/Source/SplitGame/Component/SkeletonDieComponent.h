// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkeletonDieComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLITGAME_API USkeletonDieComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkeletonDieComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling")
		float MaxFallingDistance = 1000;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Falling")
		bool IsAboveDangerDistance = false;

	 
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartRecord();

	void EndRecord();

	void Reset();

private:
	bool bIsRecording = false;
	float OldLocationZ = 0;
	float CurrentFallingDistance = 0;
};
