// Fill out your copyright notice in the Description page of Project Settings.

#include "SplitGameUserSettings.h"

void USplitGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
}

void USplitGameUserSettings::SetInvertControls(bool i_bInvertControl)
{
	b_AreControlsInverted = i_bInvertControl;
}

void USplitGameUserSettings::SetMasterVolume(float i_VolumeLevel)
{
	m_MasterVolume = i_VolumeLevel;
}
