// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

/**
 * 
 */

class USplitGameUserSettings;
UCLASS()
class SPLITGAME_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
private:
	static bool bIsControlsInverted;
	static bool bIsFirstTime;
	FIntPoint m_CurrentResolution;
	static float m_CurrentVolume;
	EWindowMode::Type m_CurrentWindowType;
	static class USplitGameUserSettings* gSettings;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
		static bool AreControlsInverted();

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetVolume(float i_Volume);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void ApplySettings(FIntPoint i_ScreenResolution, bool i_bInvertControls, EWindowMode::Type i_WindowMode);

	UFUNCTION(BlueprintCallable, Category = "Settings")
		FIntPoint GetCurrentScreenResolution();

	UFUNCTION(BlueprintCallable, Category = "Settings")
		static float GetCurrentMasterVolume();

	UFUNCTION(BlueprintCallable, Category = "Settings")
		static bool IsFirstTime() { return bIsFirstTime; }

	//This function loads all settings apart from controls inversion.
		UFUNCTION(BlueprintCallable, Category = "Settings")
		static void LoadExistingGameData();
};
