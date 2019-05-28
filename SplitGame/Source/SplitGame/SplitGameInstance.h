// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SplitGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API USplitGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USplitGameInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, Category = "Audio System", meta = (DisplayName = "Audio System"))
	AActor* mAudioSystem = nullptr;

	UFUNCTION(BlueprintCallable)
	class ASplitGameCharacter* GetCurrentCharacter();

	UFUNCTION(BlueprintCallable)
	class ASkeletonCharacter* GetSkeleton();

	UFUNCTION(BlueprintPure)
		bool IsLevelFinished();


protected: 
	virtual void OnStart() override;

private:
	UClass* mAudioSystemClass;

	
	
};
