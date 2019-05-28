// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Image.h"
#include "CustomMapButton.generated.h"

/**
*
*/

class UTexture2D;
class USoundWave;
UCLASS()
class SPLITGAME_API UCustomMapButton : public UButton
{
	GENERATED_BODY()

public:

	UCustomMapButton();

	UCustomMapButton(const class FObjectInitializer& ObjectInitializer);

	virtual ~UCustomMapButton();

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
		bool IsLevelUnlocked();

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
		void UpdateButtonState(TArray<FString> i_UnlockLevels);

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
		bool IsLevelCompleted() const;

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
		static int GetMaxMapLevelToScrollTo() { return m_MaxMapLevelToScrollTo; }

	UFUNCTION(BlueprintCallable, Category = "Custom Button")
		static void SetMaxMapLevelToScrollTo(int val) { m_MaxMapLevelToScrollTo = val; }

	UFUNCTION(BlueprintCallable, Category = "CustomButton")
		TArray<FString> GetLevelsNeededToUnlock() { return m_LevelsToUnlock; }

	void SetIsEnabled(bool i_Enabled) override;

	void PostLoad() override;

public:
	static int m_MaxMapLevelToScrollTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Details")
		TArray<FString> m_LevelsToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Details")
		FString m_LevelToOpenWhenClicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Details")
		UTexture2D* m_OutputTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Details")
		USoundWave* m_HoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Details")
		USoundWave* m_PressedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int m_MapLevel;


private:
	TArray<FString> m_UnlockedLevels;
	UFUNCTION()
		void CallLevel();
};