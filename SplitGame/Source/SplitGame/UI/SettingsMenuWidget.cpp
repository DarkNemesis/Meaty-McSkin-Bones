// Fill out your copyright notice in the Description page of Project Settings.

#include "SettingsMenuWidget.h"
#include "Engine/GameEngine.h"
#include "Runtime/Engine/Classes/GameFramework/GameUserSettings.h"
#include "SplitGameUserSettings.h"

bool USettingsMenuWidget::bIsControlsInverted = false;

bool USettingsMenuWidget::bIsFirstTime = false;

float USettingsMenuWidget::m_CurrentVolume = 0.0f;

USplitGameUserSettings* USettingsMenuWidget::gSettings = nullptr;

void USettingsMenuWidget::LoadExistingGameData()
{
	if (!gSettings)
	{
		gSettings = Cast<USplitGameUserSettings>(GEngine->GetGameUserSettings());
	}
	gSettings->LoadSettings(true);
	bIsControlsInverted = gSettings->GetInvertControls();
	m_CurrentVolume = gSettings->GetMasterVolume();
	bIsFirstTime = gSettings->IsGamePlayedFirstTime();
	if (bIsFirstTime)
	{
		gSettings->SetFirstTimeFlag(false);
		gSettings->ApplySettings(false);
	}
}

bool USettingsMenuWidget::AreControlsInverted()
{
	return bIsControlsInverted;
}

void USettingsMenuWidget::SetVolume(float i_Volume)
{
	m_CurrentVolume = i_Volume;
}

void USettingsMenuWidget::ApplySettings(FIntPoint i_ScreenResolution, bool i_bInvertControls, EWindowMode::Type i_WindowMode)
{
	if (!gSettings)
	{
		gSettings = Cast<USplitGameUserSettings>(GEngine->GetGameUserSettings());
	}
	if (gSettings)
	{
		bIsControlsInverted = i_bInvertControls;
		gSettings->SetInvertControls(bIsControlsInverted);
		gSettings->SetMasterVolume(m_CurrentVolume);
		gSettings->SetScreenResolution(FIntPoint(i_ScreenResolution.X, i_ScreenResolution.Y));
		gSettings->SetFullscreenMode(i_WindowMode);
		gSettings->ConfirmVideoMode();
		gSettings->ApplySettings(false);
		m_CurrentResolution = i_ScreenResolution;
		m_CurrentWindowType = i_WindowMode;
	}
}

FIntPoint USettingsMenuWidget::GetCurrentScreenResolution()
{
	if (!gSettings)
	{
		gSettings = Cast<USplitGameUserSettings>(GEngine->GetGameUserSettings());
	}
	return gSettings->GetScreenResolution();
}

float USettingsMenuWidget::GetCurrentMasterVolume()
{
	if (!gSettings)
	{
		gSettings = Cast<USplitGameUserSettings>(GEngine->GetGameUserSettings());
	}
	return gSettings->GetMasterVolume();
}
