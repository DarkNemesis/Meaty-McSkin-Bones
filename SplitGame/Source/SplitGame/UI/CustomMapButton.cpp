// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomMapButton.h"
#include "MapMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Level/LevelManager.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundWave.h"


int UCustomMapButton::m_MaxMapLevelToScrollTo;

UCustomMapButton::UCustomMapButton()
{
	OnClicked.AddDynamic(this, &UCustomMapButton::CallLevel);
}

UCustomMapButton::UCustomMapButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	m_OutputTexture = NewObject<UTexture2D>();

	m_HoverSound = LoadObject<USoundWave>(nullptr, TEXT("/Game/SFX/UI_Hover_over/MMSB_MENU_MAIN_SWITCH_002.MMSB_MENU_MAIN_SWITCH_002"));

	m_PressedSound = LoadObject<USoundWave>(nullptr, TEXT("/Game/SFX/UI_Select/MMSB_MENU_MAIN_SELECT_003.MMSB_MENU_MAIN_SELECT_003"));

	OnClicked.AddDynamic(this, &UCustomMapButton::CallLevel);
}

UCustomMapButton::~UCustomMapButton()
{
}

bool UCustomMapButton::IsLevelUnlocked()
{
	if (m_LevelsToUnlock.Num() <= 0)
	{
		return true;
	}
	auto unlockedLevels = m_UnlockedLevels;
	for (auto neededUnlockLevels : m_LevelsToUnlock)
	{
		bool isFound = false;
		for (auto unlockedLevel : unlockedLevels)
		{
			if (neededUnlockLevels == unlockedLevel)
			{
				isFound = true;
				break;
			}
		}
		if (!isFound)
		{
			return false;
		}
	}

	return true;
}

void UCustomMapButton::UpdateButtonState(TArray<FString> i_UnlockLevels)
{
	m_UnlockedLevels = ALevelManager::GetUnlockedLevels().Num() > 0 ? ALevelManager::GetUnlockedLevels() : i_UnlockLevels;
	bool isLevelUnlocked = IsLevelUnlocked();
	bool isLevelCompleted = IsLevelCompleted();
	SetIsEnabled(isLevelUnlocked || isLevelCompleted);
	if (isLevelUnlocked && (m_MapLevel > m_MaxMapLevelToScrollTo))
	{
		m_MaxMapLevelToScrollTo = m_MapLevel;
	}
	if (isLevelCompleted)
	{
		FButtonStyle* buttonStyle = &WidgetStyle;
		FSlateBrush* normalStyle = &(buttonStyle->Normal);
		normalStyle->SetResourceObject(Cast<UObject>(m_OutputTexture));
		return;
	}
}

bool UCustomMapButton::IsLevelCompleted() const
{
	auto unlockedLevels = m_UnlockedLevels;
	for (auto unlockedLevel : unlockedLevels)
	{
		if (m_LevelToOpenWhenClicked == unlockedLevel) return true;
	}
	return false;
}

void UCustomMapButton::SetIsEnabled(bool i_Enabled)
{
	Super::SetIsEnabled(i_Enabled);
}

void UCustomMapButton::PostLoad()
{
	//TODO: Fix the button loading after map issue.


	Super::PostLoad();
	FButtonStyle* buttonStyle = &WidgetStyle;
	FSlateSound* pressedSound = &(buttonStyle->PressedSlateSound);
	pressedSound->SetResourceObject(Cast<UObject>(m_PressedSound));
	FSlateSound* hoveredSound = &(buttonStyle->HoveredSlateSound);
	hoveredSound->SetResourceObject(Cast<UObject>(m_HoverSound));
	RenderTransform.Scale = FVector2D(0.8f, 0.8f);
}

void UCustomMapButton::CallLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*m_LevelToOpenWhenClicked), true);
}