// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelName.h"


void ULevelName::SetTexture(FString& i_LevelName)
{
	if (m_LevelNameTexturesMap.Find(i_LevelName) !=nullptr)
	{
		m_OutputTexture = m_LevelNameTexturesMap[i_LevelName];
	}
	else
	{
		m_OutputTexture = m_LevelNameTexturesMap["01_bones-skin"];
	}
}
