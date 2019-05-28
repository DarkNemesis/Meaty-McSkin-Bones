// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "SFXComponent.generated.h"

class USoundBase;
class USceneComponent;

USTRUCT(BlueprintType)
struct FSfxSources
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class USoundBase*> sources;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPLITGAME_API USFXComponent : public UAudioComponent
{	
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundBase*> mAudio;	

public:
	enum SFXAssetType
	{
		SoundWave,
		SoundCue
	};

	USFXComponent();
	~USFXComponent();

	void AddAudio(FName AudioFile, SFXAssetType AssetType);
	void AddAudio(USoundBase* AudioAsset);

	virtual void Play(float StartTime = 0.f);
	virtual void Stop();
};
