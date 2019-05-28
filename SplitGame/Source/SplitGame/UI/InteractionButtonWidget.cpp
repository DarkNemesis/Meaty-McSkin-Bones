// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionButtonWidget.h"


void UInteractionButtonWidget::ShowInteractionButton(EInteractionTypes i_InteractionType)
{
	switch (i_InteractionType)
	{
	case EInteractionTypes::CharacterInteraction:
		m_OutputTexture = m_InteractionTexture;
		break;
	case EInteractionTypes::WallUpInteraction:
		m_OutputTexture = m_ClimbTexture;
		break;
	case EInteractionTypes::WallDownInteraction:
		m_OutputTexture = m_ClimbDownTexture;
		break;
	case EInteractionTypes::SwitchInteraction:
		m_OutputTexture = m_SwitchTexture;
		break;
	case EInteractionTypes::PushInteraction:
		m_OutputTexture = m_PushTexture;
		break;
	case EInteractionTypes::GlideInteraction:
		m_OutputTexture = m_GlideTexture;
		break;
	case EInteractionTypes::BoomerangInteraction:
		m_OutputTexture = m_BoomerangTexture;
		break;
	case EInteractionTypes::CameraControl:
		m_OutputTexture = m_CameraTexture;
		break;
	case EInteractionTypes::Cancel:
		m_OutputTexture = m_CancelTexture;
		break;
	case EInteractionTypes::GrabInteraction:
		m_OutputTexture = m_GrabBoneTexture;
		break;
	case EInteractionTypes::ParachuteInteraction:
		m_OutputTexture = m_ParachuteTexture;
		break;
	case EInteractionTypes::ClimbMuscleInteraction:
		m_OutputTexture = m_ClimbMuscleTexture;
		break;
	}
}