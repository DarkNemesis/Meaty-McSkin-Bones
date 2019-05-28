// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelName.generated.h"

/**
 * 
 */
class UTexture2D;
UCLASS()
class SPLITGAME_API ULevelName : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Level Map")
		void SetTexture(FString& i_LevelName);

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		TMap<FString, UTexture2D*> m_LevelNameTexturesMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_OutputTexture;
	
};
