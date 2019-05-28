// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "ScreenshotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPLITGAME_API UScreenshotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScreenShot Tool Properties")
		UTexture2D* m_ScreenshotTexture;

		TArray<FColor> bitMapData;
		int32 X, Y;

public:
	UFUNCTION(BlueprintCallable, Category = "Screenshot Function")
		void SubmitBugData(UImage* i_Image, FString i_BugReport);
};
