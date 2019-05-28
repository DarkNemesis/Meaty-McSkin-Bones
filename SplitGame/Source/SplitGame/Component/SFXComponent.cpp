#include "SFXComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

USFXComponent::USFXComponent()
{	
	bAutoActivate = false;
	SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

void USFXComponent::AddAudio(FName AudioFile, SFXAssetType AssetType)
{
	if (AssetType == SFXAssetType::SoundWave)
	{
		ConstructorHelpers::FObjectFinder<USoundWave> wave(*AudioFile.ToString());
		mAudio.Push(Cast<USoundBase>(wave.Object));
	}
	else
	{
		ConstructorHelpers::FObjectFinder<USoundCue> cue(*AudioFile.ToString());
		mAudio.Push(Cast<USoundBase>(cue.Object));
	}
}

void USFXComponent::AddAudio(USoundBase * AudioAsset)
{
	mAudio.Push(AudioAsset);
}

void USFXComponent::Play(float StartTime)
{
#ifndef UE_SHIPPING
	if (mAudio.Num() <= 0)
		return;
#endif
	// Attach our sound cue to the SoundComponent (outside the constructor)
	if (!IsPlaying())
	{
		int index = FMath::RandRange(0, mAudio.Num() - 1);

		if (mAudio[index]->IsValidLowLevelFast())
			SetSound(mAudio[index]);
				
		UAudioComponent::Play();		
	}
}

void USFXComponent::Stop()
{
	if (IsPlaying())
	{
		UAudioComponent::Stop();
	}
}

USFXComponent::~USFXComponent()
{
}
