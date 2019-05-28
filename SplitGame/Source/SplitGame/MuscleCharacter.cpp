// Fill out your copyright notice in the Description page of Project Settings.

#include "MuscleCharacter.h"
#include "Environment/EnvObject.h"
#include "Environment/EnvPushAndPull.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SkeletonCharacter.h"
#include "Boomerang.h"
#include "Level/LevelManager.h"
#include <Misc/OutputDeviceNull.h>
#include "Engine/World.h"
#include "Environment/EnvHotSurface.h"
#include "GameFramework/PlayerController.h"
#include "Component/SFXComponent.h"
#include "UI/InteractionButtonWidget.h"

// Sets default values
AMuscleCharacter::AMuscleCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	for (int i = 0; i < (uint8)EMuscleSFX::Count; i++)
	{
		FName name(TEXT("SFX Component"), i);
		auto sfxComponent = CreateDefaultSubobject<USFXComponent>(name);
		sfxComponent->SetupAttachment(RootComponent);
		mSFXComponents.Add((EMuscleSFX)i, sfxComponent);
	}

}

AMuscleCharacter::~AMuscleCharacter()
{
}

// Called when the game starts or when spawned
void AMuscleCharacter::BeginPlay()
{
	Super::BeginPlay();	

	for (auto It = mSoundSources.CreateIterator(); It; ++It)
	{
		FSfxSources& value = It->Value;
		auto sfxComponent = mSFXComponents[It->Key];
		for (int i = 0; i < value.sources.Num(); i++)
		{
			sfxComponent->AddAudio(value.sources[i]);
		}
	}

}

void AMuscleCharacter::Jump()
{
	if (!bIsCarrySkeleton)
	{
		//auto movement = GetCharacterMovement();
		//movement->StopMovementImmediately();		
		Super::Jump();
	}
}

void AMuscleCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == MOVE_Walking && GetController()->IsA(APlayerController::StaticClass()) && 
		!(mInteractingEnv && mInteractingEnv->IsA(AEnvHotSurface::StaticClass())))
	{
		// save
		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (levelManager)
		{
			if (levelManager->m_SaveSystem && !IsDead())
			{
				FOutputDeviceNull ar;
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("SaveLevelCharacters 0"), ar, NULL, true);
			}
		}
	}

}

void AMuscleCharacter::OnCharInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASplitGameCharacter::OnCharInteractiveBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor->IsA(ASplitGameCharacter::StaticClass()) && OtherActor != this && !IsCharInteracting())
	{
		if (OtherActor->IsA(ASkeletonCharacter::StaticClass()) && GetWorld()->GetFirstPlayerController()->GetCharacter()->IsA(AMuscleCharacter::StaticClass()))
		{
			ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
			if (levelManager)
			{
				levelManager->ShowInteractionWidgetForType(EInteractionTypes::GrabInteraction);
			}
		}
	}
}

void AMuscleCharacter::OnCharInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA(ASplitGameCharacter::StaticClass()) || OtherActor == this)
		return;

	ASplitGameCharacter::OnCharInteractiveEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if (levelManager)
	{
		levelManager->RemoveInteractionWidget();
	}
}

// Called every frame
void AMuscleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto movement = GetCharacterMovement();
	if (movement->IsFalling())
	{
		movement->GravityScale = 6.f / 3.f;
	}
	else
	{
		movement->GravityScale = 1.f / 3.f;
	}

	GetPushingDirection();
}

void AMuscleCharacter::OnEnvInteractiveBegin(AEnvObject* EnvObject)
{
	if(mInteractingEnv == nullptr)
	{
		mInteractingEnv = EnvObject;
	}	
}

void AMuscleCharacter::OnEnvInteractiveEnd(AEnvObject* EnvObject)
{
	if (mInteractingEnv != nullptr && mInteractingEnv == EnvObject)
	{
		mInteractingEnv = nullptr;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void AMuscleCharacter::OnInteractionInputPressed()
{
	if (auto skeleton = Cast<ASkeletonCharacter>(mInteractingChar))
	{
		if (!bIsHoldingBone && !bIsCarrySkeleton && skeleton->isBoomerangReady)
		{
			bIsHoldingBone = true;
			skeleton->isBoomerangReady = false;
			ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
			if (levelManager)
			{
				levelManager->ShowInteractionWidgetForType(EInteractionTypes::Cancel);
			}
		}
	}
}

void AMuscleCharacter::OnInteractionInputReleased()
{
}

void AMuscleCharacter::OnActionInputPressed()
{
	if (bIsHoldingBone)
	{
		ABoomerang::Shoot(this);
		bIsHoldingBone = false;
		return;
	}

	if (mInteractingEnv)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;

		auto rot = GetActorRotation();

		if (GetActorLocation().Y < mInteractingEnv->GetActorLocation().Y)
			rot.Yaw = -90.0f;
		else
			rot.Yaw = 90.f;
		
		SetActorRotation(rot);		
	}
}

void AMuscleCharacter::OnActionInputReleased()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AMuscleCharacter::OnActionInputHeld()
{	
	if (!bIsCarrySkeleton && mInteractingEnv && bActionInputState && GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		mInteractingEnv->Action(this);	

		if (Cast<AEnvPushAndPull>(mInteractingEnv))
		{
			GetCharacterMovement()->MaxWalkSpeed = 250.f;
		}
	}
}

int AMuscleCharacter::GetPushingDirection()
{
	if (mInteractingEnv && Cast<AEnvPushAndPull>(mInteractingEnv) && bActionInputState)
	{
		if (mInteractingEnv->GetActorLocation().Y < GetActorLocation().Y)
			return 1;
		else
			return -1;
	}
	else return 0;
}

void AMuscleCharacter::PlaySFX(EMuscleSFX type)
{
	if (mSFXComponents.Contains(type))
	{
		mSFXComponents[type]->Play();
	}
}

void AMuscleCharacter::StopSFX(EMuscleSFX type)
{
	if (mSFXComponents.Contains(type))
	{
		mSFXComponents[type]->Stop();
	}

}

bool AMuscleCharacter::IsCharInteracting()
{
	return bIsCarrySkeleton || bIsHoldingBone;
}

