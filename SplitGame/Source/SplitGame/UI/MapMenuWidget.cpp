// Fill out your copyright notice in the Description page of Project Settings.

#include "MapMenuWidget.h"
#include "Engine/Engine.h"
#include "CustomMapButton.h"

int UMapMenuWidget::m_ScrollLevel;

float UMapMenuWidget::m_PreviousMovementVal = 0;


UMapMenuWidget::UMapMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	m_ScrollLevel = 0;
}

void UMapMenuWidget::PostLoad()
{
	Super::PostLoad();
}

void UMapMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!b_ShouldScroll) return;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(tempMovementVal));

	m_CurrentScrollPosition = m_ScrollImage->RenderTransform.Translation;
	if (!m_OnboardingCanvasPanel || !m_ScrollImage || !m_ForeImage) return;
	if (tempMovementVal <= 0) return;
	if (m_CurrentScrollPosition.X == 0 || m_CurrentScrollPosition.X < m_MaxScrollPosition)
	{
		if (m_MovementValue >= m_MaxScrollPosition)
		{
			if (m_CurrentScrollPosition.X < m_MaxScrollPosition)
			{
				auto temp = m_ScrollImage->RenderTransform.Translation;
				m_ScrollImage->RenderTransform.Translation = FVector2D(temp.X + InDeltaTime * m_MaxMovementPosition, temp.Y);
				m_ScrollImage->RenderTransform.Angle = InDeltaTime;
				m_ScrollImage->SynchronizeProperties();
				temp = m_ForeImage->RenderTransform.Translation;
				m_ForeImage->RenderTransform.Translation = FVector2D(temp.X + InDeltaTime * m_MaxMovementPosition, temp.Y);
				m_ForeImage->SynchronizeProperties();
				tempMovementVal -= (InDeltaTime * m_MaxMovementPosition);
			}
			else
			{
				auto panelTranslation = m_OnboardingCanvasPanel->RenderTransform.Translation;
				m_OnboardingCanvasPanel->RenderTransform.Translation = FVector2D(panelTranslation.X - (InDeltaTime * m_MaxMovementPosition), panelTranslation.Y);
				m_OnboardingCanvasPanel->SynchronizeProperties();
				tempMovementVal -= (InDeltaTime * m_MaxMovementPosition);
			}
		}
		else
		{
			auto temp = m_ScrollImage->RenderTransform.Translation;
			m_ScrollImage->RenderTransform.Translation = FVector2D(temp.X + InDeltaTime * m_MaxMovementPosition, temp.Y);
			m_ScrollImage->RenderTransform.Angle = InDeltaTime;
			m_ScrollImage->SynchronizeProperties();
			temp = m_ForeImage->RenderTransform.Translation;
			m_ForeImage->RenderTransform.Translation = FVector2D(temp.X + InDeltaTime * m_MaxMovementPosition, temp.Y);
			m_ForeImage->SynchronizeProperties();
			tempMovementVal -= (InDeltaTime * m_MaxMovementPosition);
		}
	}
	else
	{
		auto panelTranslation = m_OnboardingCanvasPanel->RenderTransform.Translation;
		m_OnboardingCanvasPanel->RenderTransform.Translation = FVector2D(panelTranslation.X - (InDeltaTime * m_MaxMovementPosition), panelTranslation.Y);
		m_OnboardingCanvasPanel->SynchronizeProperties();
		tempMovementVal -= (InDeltaTime * m_MaxMovementPosition);
	}

}

void UMapMenuWidget::SetImages(UCanvasPanel* onboardingPanel, UCustomMapImage* scrollImage, UCustomMapImage* fogImage)
{

	//TODO: Add fix to load buttons after map is loaded - Post GDC.
	m_OnboardingCanvasPanel = onboardingPanel;
	m_ScrollImage = scrollImage;
	m_ForeImage = fogImage;
	b_ShouldScroll = true;
	m_ScrollLevel = UCustomMapButton::m_MaxMapLevelToScrollTo;
	m_ScrollImage->RenderTransform.Translation = FVector2D(m_PreviousMovementVal, m_ScrollImage->RenderTransform.Translation.Y);
	m_ScrollImage->SynchronizeProperties();
	m_ForeImage->RenderTransform.Translation = FVector2D(m_PreviousMovementVal, m_ForeImage->RenderTransform.Translation.Y);
	m_ForeImage->SynchronizeProperties();
	m_CurrentScrollPosition = m_ScrollImage->RenderTransform.Translation;
	m_MovementValue = m_ScrollLevel * m_MaxMovementPosition - m_PreviousMovementVal;
	tempMovementVal = m_MovementValue;
	m_PreviousMovementVal = m_MovementValue > 0 ? m_MovementValue : m_PreviousMovementVal;
}
