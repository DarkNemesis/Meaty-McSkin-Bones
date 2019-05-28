// Fill out your copyright notice in the Description page of Project Settings.

#include "SplitGameInstance.h"
#include "SplitGameCharacter.h"
#include "SkeletonCharacter.h"
#include "Engine/World.h"
#include "Level/LevelManager.h"
#include "Engine/World.h"
#include "Environment/EnvFlag.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


USplitGameInstance::USplitGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

ASplitGameCharacter * USplitGameInstance::GetCurrentCharacter()
{
	ASplitGameCharacter* pCharacter = nullptr;
	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if (levelManager != nullptr)
	{
		pCharacter = levelManager->GetCurrentCharacter();
	}

	return pCharacter;
}

ASkeletonCharacter * USplitGameInstance::GetSkeleton()
{
	ASkeletonCharacter* pSkeletonCharacter = nullptr;
	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if (levelManager != nullptr)
	{
		auto actors = levelManager->GetAllActors();
		for (int i = 0; i < actors.Num(); ++i)
		{
			pSkeletonCharacter = Cast<ASkeletonCharacter>(actors[i]);
			if (pSkeletonCharacter != nullptr)
				return pSkeletonCharacter;
		}
	}
	return nullptr;
}

bool USplitGameInstance::IsLevelFinished()
{
	check(GetWorld());
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvFlag::StaticClass(), actors);
	if(actors.Num() > 0)
	{
		return Cast<AEnvFlag>(actors[0])->isFinished;
	}
	
	return false;
}

void USplitGameInstance::OnStart()
{
	// get audio system class and spawn one
	UClass* result = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/BP_AudioManager.BP_AudioManager_C"));
	check(result);
	mAudioSystemClass = result;
	mAudioSystem = GetWorld()->SpawnActor<AActor>(mAudioSystemClass, FTransform());

}
