// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvHotSurface.h"
#include "Engine/World.h"
#include "Level/LevelManager.h"
#include <Misc/OutputDeviceNull.h>
#include "Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "SplitGame/MuscleCharacter.h"
#include "SplitGame/SkeletonCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Component/SFXComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnvHotSurface::AEnvHotSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set collider as the root component
	m_collider = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = m_collider;
	m_collider->InitBoxExtent(m_Extent);
	m_collider->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	m_collider->SetCollisionResponseToAllChannels(ECR_Block);

	//create mesh component
	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	m_mesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (meshAsset.Succeeded())
	{
		m_mesh->SetStaticMesh(meshAsset.Object);
	}

	//setup trigger
	m_trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	m_trigger->SetupAttachment(RootComponent);
	m_trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_trigger->InitBoxExtent(m_TriggerExtent);
	m_trigger->OnComponentBeginOverlap.AddDynamic(this, &AEnvHotSurface::OnInteractiveBegin);
	m_trigger->OnComponentEndOverlap.AddDynamic(this, &AEnvHotSurface::OnInteractiveEnd);

	m_leftEnterTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftEnterComponent"));
	m_leftEnterTrigger->SetupAttachment(RootComponent);
	m_leftEnterTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_leftEnterTrigger->InitBoxExtent(m_DefaultEnterTriggerExtent);	
	m_leftEnterTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEnvHotSurface::SetRespawnLocation);

	m_rightEnterTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RightEnterComponent"));
	m_rightEnterTrigger->SetupAttachment(RootComponent);
	m_rightEnterTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_rightEnterTrigger->InitBoxExtent(m_DefaultEnterTriggerExtent);	
	m_rightEnterTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEnvHotSurface::SetRespawnLocation);


	mSFXSliding = CreateDefaultSubobject<USFXComponent>(TEXT("SlidingAudio"));
	mSFXSliding->SetupAttachment(RootComponent);
	mSFXSliding->AddAudio("SoundWave'/Game/SFX/Melted_Cheese_Ambient/MMSB_OBJ_MELTED_CHEESE_BUBBLES_003.MMSB_OBJ_MELTED_CHEESE_BUBBLES_003'", USFXComponent::SFXAssetType::SoundWave);


}

// Called when the game starts or when spawned
void AEnvHotSurface::BeginPlay()
{
	Super::BeginPlay();

	//auto y = m_trigger->GetUnscaledBoxExtent().Y;
	//auto ly = m_leftEnterTrigger->GetUnscaledBoxExtent().Y;
	//auto ry = m_rightEnterTrigger->GetUnscaledBoxExtent().Y;

	//auto scaleY = GetActorScale3D().Y;
	//m_leftEnterTrigger->SetWorldScale3D(FVector(1.0f, 1.0f / scaleY, 1.0f));
	//m_rightEnterTrigger->SetWorldScale3D(FVector(1.0f, 1.0f / scaleY, 1.0f));

	//m_leftEnterTrigger->SetRelativeLocation(FVector(0, -(ly + y), 0));
	//m_rightEnterTrigger->SetRelativeLocation(FVector(0, ry + y, 0));
	

	m_bIsSurfaceOn = true;	
	mSFXSliding->Play();
}

// Called every frame
void AEnvHotSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AEnvHotSurface::TurnOnSurface(bool surfaceOn)
{
	m_bIsSurfaceOn = surfaceOn;
	if (m_bIsSurfaceOn)
		mSFXSliding->Play();
	else
		mSFXSliding->Stop();
}

bool AEnvHotSurface::KnobActivated()
{
	m_bIsSurfaceOn = true;
	m_mesh->SetMaterial(0, (UMaterialInterface*)m_surfaceOnMaterial);
	return m_bIsSurfaceOn;
}

bool AEnvHotSurface::KnobDeactivated()
{
	m_bIsSurfaceOn = false;
	m_mesh->SetMaterial(0, (UMaterialInterface*)m_surfaceOffMaterial);
	return m_bIsSurfaceOn;
}

void AEnvHotSurface::OnInteractiveBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!m_bIsSurfaceOn)
		return;

	auto* actor = Cast<ASplitGameCharacter>(OtherActor);
	if (actor != nullptr && Cast<ASkeletonCharacter>(OtherActor) == nullptr)
	{
		//if other actor is muscle or skin, it will die
		if (OverlappedComponent->GetFName() == ACharacter::CapsuleComponentName || OtherComp->GetFName() == ACharacter::CapsuleComponentName)
		{
			auto skinCctor = Cast<ASkinCharacter>(actor);
			if (skinCctor != nullptr)
			{
				ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
				if (levelManager)
				{
					auto actors = levelManager->GetAllActors();
					for (int i = 0; i < actors.Num(); ++i)
					{
						auto a = Cast<ASkeletonCharacter>(actors[i]);
						if (a != nullptr && a->IsCarringSkin())
						{
							a->SetDead(true);
						}
					}
				}
			}
			actor->SetDead(true);			
		}
	}
}

void AEnvHotSurface::OnInteractiveEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void AEnvHotSurface::SetRespawnLocation(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto* actor = Cast<ASplitGameCharacter>(OtherActor);
	if (actor != nullptr && Cast<ASkeletonCharacter>(OtherActor) == nullptr)
	{
		//if trigger, save position
		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (levelManager && actor->GetCharacterMovement()->MovementMode != MOVE_Falling &&(OverlappedComponent->GetFName() == ACharacter::CapsuleComponentName || OtherComp->GetFName() == ACharacter::CapsuleComponentName))
		{
			if (levelManager->m_SaveSystem)
			{
				FOutputDeviceNull ar;
				if (Cast<AMuscleCharacter>(OtherActor) != nullptr)
				{
					levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("SaveLevelCharacters 0"), ar, NULL, true);
				}
				else if (Cast<ASkinCharacter>(OtherActor) != nullptr)
				{
					auto skin = Cast<ASkinCharacter>(OtherActor);
					if(!skin->GetIsCharacterFloating())
						levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("SaveLevelCharacters 2"), ar, NULL, true);
				}
			}
		}
	}
}

