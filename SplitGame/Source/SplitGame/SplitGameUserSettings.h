// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SplitGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API USplitGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	void SetInvertControls(bool i_bInvertControl);
	bool GetInvertControls() { return b_AreControlsInverted; }
	void SetMasterVolume(float i_VolumeLevel);
	bool IsGamePlayedFirstTime() { return b_IsFirstTime; }
	void SetFirstTimeFlag(bool i_FirstTime) { b_IsFirstTime = i_FirstTime; }
	UFUNCTION(BlueprintCallable, Category = "Game settings")
	float GetMasterVolume() { return m_MasterVolume; }

private:
	UPROPERTY(config)
	bool b_AreControlsInverted;

	UPROPERTY(config)
	float m_MasterVolume = 1.0f;

	UPROPERTY(config)
	bool b_IsFirstTime = true;
	
};
