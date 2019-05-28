// Fill out your copyright notice in the Description page of Project Settings.

#include "SkinCharacter.h"
#include "Camera/CameraComponent.h"
#include "MuscleCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/SkinCatapultComponent.h"
#include "SkeletonCharacter.h"
#include "Engine/World.h"
#include "Level/LevelManager.h"
#include <Misc/OutputDeviceNull.h>
#include "Engine/GameEngine.h"
#include "Component/SFXComponent.h"
#include <Engine/Classes/Components/SkeletalMeshComponent.h>

// Sets default values
ASkinCharacter::ASkinCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	m_InAirVentAudio = CreateDefaultSubobject<USFXComponent>(TEXT("AirVentAudio"));
	m_InAirVentAudio->SetupAttachment(RootComponent);
	m_InAirVentAudio->AddAudio("SoundWave'/Game/SFX/Skin_Catch_Air/newCatch_Airvent.newCatch_Airvent'", USFXComponent::SFXAssetType::SoundWave);

	m_GlideAudio = CreateDefaultSubobject<USFXComponent>(TEXT("GlideAudio"));
	m_GlideAudio->SetupAttachment(RootComponent);
	m_GlideAudio->AddAudio("SoundWave'/Game/SFX/Skin_Glide/new_skin_glide.new_skin_glide'", USFXComponent::SFXAssetType::SoundWave);

	mSlideSFXComponent = CreateDefaultSubobject<USFXComponent>(TEXT("SlideSFX"));
	mSlideSFXComponent->SetupAttachment(RootComponent);
	mSlideSFXComponent->AddAudio("SoundWave'/Game/SFX/Skin_slides_along_ground/Edited_Skin_Crawl.Edited_Skin_Crawl'", USFXComponent::SFXAssetType::SoundWave);

	mDeadAudio = CreateDefaultSubobject<USFXComponent>(TEXT("DeadSFX"));
	mDeadAudio->SetupAttachment(RootComponent);
	mDeadAudio->AddAudio("SoundWave'/Game/SFX/Skin_dies_VO/MMSB_CHR_SKIN_DIE_001__2___1_.MMSB_CHR_SKIN_DIE_001__2___1_'", USFXComponent::SFXAssetType::SoundWave);
	mDeadAudio->AddAudio("SoundWave'/Game/SFX/Skin_dies_VO/MMSB_CHR_SKIN_DIE_005__1_.MMSB_CHR_SKIN_DIE_005__1_'", USFXComponent::SFXAssetType::SoundWave);

	mBurnAudio = CreateDefaultSubobject<USFXComponent>(TEXT("BurnSFX"));
	mBurnAudio->SetupAttachment(RootComponent);	
	mBurnAudio->AddAudio("SoundWave'/Game/SFX/Skin_Burns/MMSB_CHR_SKIN_BURN_001.MMSB_CHR_SKIN_BURN_001'", USFXComponent::SFXAssetType::SoundWave);

	mCelebrationAudio = CreateDefaultSubobject<USFXComponent>(TEXT("CelebrationSFX"));
	mCelebrationAudio->SetupAttachment(RootComponent);
	mCelebrationAudio->AddAudio("SoundWave'/Game/SFX/Skin_celebration_VO/MMSB_CHR_SKIN_CELEB_005__1_.MMSB_CHR_SKIN_CELEB_005__1_'", USFXComponent::SFXAssetType::SoundWave);
	mCelebrationAudio->AddAudio("SoundWave'/Game/SFX/Skin_celebration_VO/MMSB_CHR_SKIN_CELEB_006.MMSB_CHR_SKIN_CELEB_006'", USFXComponent::SFXAssetType::SoundWave);
	mCelebrationAudio->AddAudio("SoundWave'/Game/SFX/Skin_celebration_VO/MMSB_CHR_SKIN_CELEB_011.MMSB_CHR_SKIN_CELEB_011'", USFXComponent::SFXAssetType::SoundWave);

	m_Balloon = CreateDefaultSubobject<USkinBalloonComponent>(TEXT("Balloon Component"));
}

void ASkinCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto lastInput = GetLastMovementInputVector();
	if (lastInput.Y != 0 && GetCharacterMovement()->MovementMode == MOVE_Walking)
	{
		mSlideSFXComponent->Play();
	}
	else
	{
		mSlideSFXComponent->Stop();
	}
}

void ASkinCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_CharacterInitialTransform = GetTransform();
}

bool ASkinCharacter::GetInputState()
{
	return bActionInputState;
}

bool ASkinCharacter::IsCharInteracting()
{
	return false;
}

void ASkinCharacter::SetIsOnAirVent(bool i_Bool)
{
	b_IsOnAirVent = i_Bool;
	if (b_IsOnAirVent)
	{
		m_InAirVentAudio->Play();
	}
}

bool ASkinCharacter::GetIsCharacterFloating()
{
	return GetCharacterMovement()->MovementMode == MOVE_Falling;
}

void ASkinCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == MOVE_Walking)
	{
		// save
		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (levelManager)
		{
			if (levelManager->m_SaveSystem)
			{
				auto mode = GetCharacterMovement()->MovementMode;
				FOutputDeviceNull ar;
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("SaveLevelCharacters 2"), ar, NULL, true);
			}
		}
	}
}

void ASkinCharacter::OnCharacterReset(ASplitGameCharacter * character)
{
	SetGravityScale(2.f / 3.f);
	m_CurrentFloatTime = 0.0f;
	Super::OnCharacterReset(character);

}

void ASkinCharacter::PlayDeadAudio()
{
	mDeadAudio->Play();
	mBurnAudio->Play();
}

void ASkinCharacter::PlayCelebrationAudio()
{
	mCelebrationAudio->Play();
}

void ASkinCharacter::OnInteractionInputPressed()
{
	/*if (Cast<AMuscleCharacter>(mInteractingChar) && m_Balloon)
	{
		m_Balloon->SetDefaultValues(this, Cast<AMuscleCharacter>(mInteractingChar));
		m_Balloon->Launch();
	}*/
}

void ASkinCharacter::OnInteractionInputReleased()
{
	/*if (mCatapult)
	{
		mCatapult->Launch();
		mCatapult->DestroyComponent();
		mCatapult = nullptr;
	}
	else if (mInteractingChar && Cast<ASkeletonCharacter>(mInteractingChar) && !mCatapult)
	{
		if (!mCatapult)
		{
			mCatapult = NewObject<USkinCatapultComponent>(this, USkinCatapultComponent::StaticClass(), "CatapultComponent");
			mCatapult->RegisterComponent();
			mCatapult->SetParameters(this, Cast<ASkeletonCharacter>(mInteractingChar));
		}
	}*/
}

void ASkinCharacter::OnInteractionInputCancelled()
{
	/*if (m_Balloon)
	{
		m_Balloon->Cancel();
	}
	if (mCatapult)
	{
		mCatapult->DestroyComponent();
		mCatapult = nullptr;
	}*/
}

void ASkinCharacter::OnSwitchFrom()
{
	/*if (mCatapult)
	{
		mCatapult->DestroyComponent();
		mCatapult = nullptr;
	}*/
}

void ASkinCharacter::OnCharInteractiveBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASplitGameCharacter::OnCharInteractiveBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}


void ASkinCharacter::OnCharInteractiveEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ASplitGameCharacter::OnCharInteractiveEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void ASkinCharacter::OnActionInputPressed()
{
	m_CurrentSkinState = eSkinStates::Balloon;
	m_GlideAudio->Play();
	if (GetCharacterMovement()->IsFalling() && !b_IsOnAirVent)
	{
		GetCharacterMovement()->AddImpulse(FVector(0, 0, -1) * GetVelocity() * m_StopForce);
	}
	SetGravityScale(m_FloatGravity);
	b_IsCharacterFloating = true;
	m_CurrentFloatTime = 0.0f;
}

void ASkinCharacter::OnActionInputReleased()
{
	if (m_CurrentSkinState != eSkinStates::Skin) m_CurrentSkinState = eSkinStates::Skin;
	m_GlideAudio->Stop();
	if (m_CurrentSkinState == eSkinStates::Skin)
	{
		SetGravityScale(2.f / 3.f);
		b_IsCharacterFloating = false;
		m_CurrentFloatTime = 0.0f;
	}
}

void ASkinCharacter::SetGravityScale(float i_GravityScale)
{
	GetCharacterMovement()->GravityScale = i_GravityScale;
}
