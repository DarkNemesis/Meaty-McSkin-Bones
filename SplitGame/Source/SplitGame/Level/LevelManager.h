// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Sound/SoundCue.h"
#include "UI/InteractionButtonWidget.h"
#include "LevelManager.generated.h"

/**
 *
 */

enum class EInteractionTypes : uint8;
//Forward Declarations
class ASplitGameCharacter;
class UScreenshotWidget;
class UMapMenuWidget;
class UAudioComponent;
UCLASS()
class SPLITGAME_API ALevelManager : public ALevelScriptActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

public:

	ALevelManager();

	void SaveCompletedLevel();
	
	TArray<ASplitGameCharacter*> GetAllActors();

	void ShowLevelMap();

	AActor* m_SaveSystem;

	ASplitGameCharacter* GetCurrentCharacter();

private:
	void NextCharacter();
	void PreviousCharacter();
	//void GenerateBugReport();
	void LoadMainMenu();

	//UFUNCTION()
	//	void BugReportHelper(int32 sizeX, int32 sizeY, const TArray<FColor>& i_Bitmap);

public:
	UPROPERTY()
		UScreenshotWidget* m_BugReportWidget;

	UPROPERTY()
		UMapMenuWidget* m_MapMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
		UInteractionButtonWidget* m_InteractionButtonWidget;

	UPROPERTY()
	class ULevelName* m_LevelNameWidget;

	UPROPERTY()
		class UUserWidget* m_VictoryWidget;

	UPROPERTY()
		class UUserWidget* m_PauseWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
		int LevelNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
		bool b_ShouldShowInteractionUI = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
		float m_InteractionTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
		bool b_IsActionButtonVisible;

	UPROPERTY()
		USoundWave* m_BackgroundWave;

	UAudioComponent* audioComp;

private:
	TSubclassOf<UUserWidget> WidgetTemplate;
	TSubclassOf<UUserWidget> MapTemplate;
	TSubclassOf<UUserWidget> InteractionButtonTemplate;
	TSubclassOf<UUserWidget> VictoryTemplate;
	TSubclassOf<UUserWidget> PauseTemplate;
	TSubclassOf<UUserWidget> LevelNameTemplate;

	float m_Timer;

	float m_BackgroundAudioTimer;

	float m_LevelInteractionTimer;

	float m_LevelNameTimer;

	float m_LevelTimer;

	float m_SequenceTimer;

	bool m_IsLevelCompleted = false;

	bool m_LevelNameShown = false;

	FString m_LevelName;

private:
	void PlayAudio();

public:

	UFUNCTION(BlueprintCallable, Category = "Level Data")
		static TArray<FString> GetUnlockedLevels();

	UFUNCTION(BlueprintCallable, Category = "Level Data")
		static void SetUnlockedLevels(TArray<FString> i_UnlockedLevels);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector mLevelBounds = FVector(9000, 9000, 9000);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void ShowInteractionWidget();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void ShowInteractionWidgetForType(EInteractionTypes i_InteractionType);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void RemoveInteractionWidget();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void ShowLevelInteractionWidget();
	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void RemoveLevelInteractionWidget();

	UFUNCTION(BlueprintCallable, Category = "Level")
		void ShowVictoryWidget();

	UFUNCTION(BlueprintCallable, Category = "Level")
		void RemoveVictoryWidget();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void ShowActionWidget();

private:
	static TArray<FString> m_UnlockedLevels;

	bool b_IsInteractionButtonVisible;

	bool b_IsLevelInteractionButtonVisible;

	bool b_InteractionStart;

	bool b_IsCancelShowed;

	bool b_LevelTimer = true;

	bool b_BackgroundStartedPlaying;

	bool b_IsPlayedFirstTime;

private:
	TArray<ASplitGameCharacter*> m_AllActorsInLevel;
	int PawnIndex;
	ASplitGameCharacter* m_CurrentCharacter;

	TArray<TArray<FColor>> m_ListOfScreenshots;
	class USFXComponent* m_SwitchCharacterAudio;

	UClass* m_SaveSystemClass;
};
