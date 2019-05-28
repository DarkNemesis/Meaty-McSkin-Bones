// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvAirVent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SplitGame/SkinCharacter.h"
#include "SkeletonCharacter.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/GameEngine.h"
#include "Component/SFXComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnvPushAndPull.h"

FName AEnvAirVent::mAirColumnColliderFName = "Foreground Effect Mesh";

namespace
{
	ASkinCharacter* skin;
}

bool AEnvAirVent::KnobActivated()
{
	b_HasTurnedOn = !b_IsTurnedOn;
	if (b_HasTurnedOn)
	{
		m_AirVentStart->Play();
		m_AirVentAmbient->Play();
	}
	else
	{
		m_AirVentAmbient->Stop();
		m_AirVentStop->Play();
	}
	ForegroundEffectMesh->SetHiddenInGame(!b_HasTurnedOn, true);
	BackgroundEffectMesh->SetHiddenInGame(!b_HasTurnedOn, true);
	return true;
}

bool AEnvAirVent::KnobDeactivated()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("knob deactivated"));
	b_HasTurnedOn = b_IsTurnedOn;
	if (b_HasTurnedOn)
	{
		m_AirVentStart->Play();
		m_AirVentAmbient->Play();
	}
	else
	{
		m_AirVentAmbient->Stop();
		m_AirVentStop->Play();
	}
	ForegroundEffectMesh->SetHiddenInGame(!b_HasTurnedOn, true);
	BackgroundEffectMesh->SetHiddenInGame(!b_HasTurnedOn, true);
	return false;
}

// Sets default values
AEnvAirVent::AEnvAirVent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(root);

	// Create box collision and static mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	Mesh->SetupAttachment(RootComponent);

	ForegroundEffectMesh = CreateDefaultSubobject<UStaticMeshComponent>(mAirColumnColliderFName);
	ForegroundEffectMesh->SetupAttachment(RootComponent);
	BackgroundEffectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT(" Background Effect Mesh"));
	BackgroundEffectMesh->SetupAttachment(RootComponent);


	//BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	//BoxCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	//BoxCollider->SetGenerateOverlapEvents(true);
	//BoxCollider->SetupAttachment(RootComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);

	OnActorBeginOverlap.AddDynamic(this, &AEnvAirVent::OnActorBeginOverlapLocal);
	OnActorEndOverlap.AddDynamic(this, &AEnvAirVent::OnActorEndOverlapLocal);

	//Audio
	m_AirVentAmbient = CreateDefaultSubobject<USFXComponent>(TEXT("AirVentAmbientAudio"));
	m_AirVentAmbient->SetupAttachment(RootComponent);
	m_AirVentAmbient->AddAudio("SoundWave'/Game/SFX/Fan_Ambient/MMSB_OBJ_AIRVENT_PROPELLOR_001.MMSB_OBJ_AIRVENT_PROPELLOR_001'", USFXComponent::SFXAssetType::SoundWave);

	m_AirVentStart = CreateDefaultSubobject<USFXComponent>(TEXT("AirVentStartAudio"));
	m_AirVentStart->SetupAttachment(RootComponent);
	m_AirVentStart->AddAudio("SoundWave'/Game/SFX/Fan_Turn_On-Off/turn_on_sound_mixdown.turn_on_sound_mixdown'", USFXComponent::SFXAssetType::SoundWave);

	m_AirVentStop = CreateDefaultSubobject<USFXComponent>(TEXT("AirVentStopAudio"));
	m_AirVentStop->SetupAttachment(RootComponent);
	m_AirVentStop->AddAudio("SoundWave'/Game/SFX/Fan_Turn_On-Off/turn_off_sound_mixdown.turn_off_sound_mixdown'", USFXComponent::SFXAssetType::SoundWave);

}

void AEnvAirVent::PostLoad()
{
	Super::PostLoad();
	b_HasTurnedOn = b_IsTurnedOn;
}

// Called when the game starts or when spawned
void AEnvAirVent::BeginPlay()
{
	Super::BeginPlay();
	m_AirVentAmbient->Play();
	ForegroundEffectMesh->SetHiddenInGame(!b_IsTurnedOn, true);
	BackgroundEffectMesh->SetHiddenInGame(!b_IsTurnedOn, true);
}

// Called every frame
void AEnvAirVent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!b_HasOverLapped) return;
	if (!b_HasTurnedOn) return;
	if (m_OverlappedSkeleton != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("HEllo"));
		if (CanPull)
		{
			m_OverlappedSkeleton->GetCharacterMovement()->AddImpulse(ForegroundEffectMesh->GetUpVector() * (-m_SkeletonForce));
			return;
		}
		m_OverlappedSkeleton->GetCharacterMovement()->AddImpulse(ForegroundEffectMesh->GetUpVector() * (m_SkeletonForce));
	}
	else if (m_OverlappedSkin != nullptr)
	{
		if (CanPull)
		{
			m_OverlappedSkin->GetCharacterMovement()->AddForce(ForegroundEffectMesh->GetUpVector() * (-m_Force));
			return;
		}
		m_OverlappedSkin->GetCharacterMovement()->AddForce(ForegroundEffectMesh->GetUpVector() * (m_Force));
	}
}

void AEnvAirVent::ChangeAirVentState(bool i_State)
{
	if (b_CanTurnOnAndOff) b_HasTurnedOn = i_State;
	if (b_HasTurnedOn)
	{
		m_AirVentStart->Play();
		m_AirVentAmbient->Play();
	}
	else
	{
		m_AirVentAmbient->Stop();
		m_AirVentStop->Play();
	}
	ForegroundEffectMesh->SetHiddenInGame(!b_HasTurnedOn, true);
	BackgroundEffectMesh->SetHiddenInGame(!b_HasTurnedOn, true);
}

void AEnvAirVent::OnActorBeginOverlapLocal(AActor* OverlappedActor, AActor* OtherActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap!"));

	b_HasOverLapped = true;
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Skeleton!"));
		ASkeletonCharacter* sCharacter = Cast<ASkeletonCharacter>(OtherActor);
		if ((sCharacter != nullptr) && (sCharacter->IsCarringSkin()))
		{
			m_OverlappedSkeleton = sCharacter;
		}
	}
	if (!m_OverlappedSkeleton)
	{
		ASkinCharacter* sCharacter = Cast<ASkinCharacter>(OtherActor);
		if (sCharacter != nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Skin"));
			m_OverlappedSkin = sCharacter;
			sCharacter->SetIsOnAirVent(true);
		}
	}
	if ((m_OverlappedSkeleton == nullptr) && (m_OverlappedSkin == nullptr))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Block"));
		auto object = Cast<AEnvPushAndPull>(OtherActor);
		if (object!=nullptr)
		{
			ChangeAirVentState(false);
		}
	}
}

void AEnvAirVent::OnActorEndOverlapLocal(AActor* OverlappedActor, AActor* OtherActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap Enf!"));
	b_HasOverLapped = false;

	auto object = Cast<AEnvPushAndPull>(OtherActor);
	if (object != nullptr)
	{
		ChangeAirVentState(b_IsTurnedOn);
	}

	if (m_OverlappedSkin != nullptr)
	{
		m_OverlappedSkin->SetIsOnAirVent(false);
		m_OverlappedSkin = nullptr;
	}
	if ((m_OverlappedSkeleton != nullptr) && (m_OverlappedSkeleton == Cast<ASkeletonCharacter>(OtherActor)))
	{
		m_OverlappedSkeleton = nullptr;
	}
}