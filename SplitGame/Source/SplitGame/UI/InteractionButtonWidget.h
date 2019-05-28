// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionButtonWidget.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EInteractionTypes: uint8
{
	CharacterInteraction,
	WallUpInteraction,
	WallDownInteraction,
	SwitchInteraction,
	PushInteraction,
	GlideInteraction,
	BoomerangInteraction,
	CameraControl,
	Cancel,
	GrabInteraction,
	ParachuteInteraction,
	ClimbMuscleInteraction,
};

UCLASS()
class SPLITGAME_API UInteractionButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_ClimbTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_ClimbDownTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_InteractionTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_SwitchTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_PushTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_GlideTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_BoomerangTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_CameraTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_CancelTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_GrabBoneTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_ParachuteTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_ClimbMuscleTexture;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
		UTexture2D* m_OutputTexture;


	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void ShowInteractionButton(EInteractionTypes i_InteractionType);
};
