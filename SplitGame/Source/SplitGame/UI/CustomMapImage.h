// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "CustomMapImage.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API UCustomMapImage : public UImage
{
	GENERATED_BODY()

public:

	virtual TSharedRef<SWidget> RebuildWidget() override;
	
};
