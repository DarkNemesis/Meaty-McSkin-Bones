// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CanvasPanel.h"
#include "Image.h"
#include "CustomMapImage.h"
#include "MapMenuWidget.generated.h"


/**
 *
 */

UCLASS()
class SPLITGAME_API UMapMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//ctor
	UMapMenuWidget(const class FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Map")
		void SetImages(UCanvasPanel* onboardingPanel , UCustomMapImage* scrollImage, UCustomMapImage* fogImage);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Levels")
	UCanvasPanel* m_OnboardingCanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Levels")
		float m_MaxMovementPosition = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Levels")
		float m_MaxScrollPosition = 1500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Data")
		UCustomMapImage* m_ScrollImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Data")
		UCustomMapImage* m_ForeImage = nullptr;

	static int m_ScrollLevel;

	static float m_PreviousMovementVal;

	bool b_ShouldScroll;

private:
	FVector2D m_CurrentScrollPosition;
	float m_MovementValue;
	float tempMovementVal;
};
