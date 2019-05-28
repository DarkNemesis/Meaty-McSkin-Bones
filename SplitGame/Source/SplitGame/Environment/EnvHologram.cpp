// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvHologram.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Component/SFXComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

bool AEnvHologram::KnobActivated()
{
	if (mIsVisible)
	{
		mSFXDisappear->Play();
		Unmaterialize();
	}
		
	else
	{
		mSFXAppear->Play();
		Materialize();		
	}
	
	return true;
}

bool AEnvHologram::KnobDeactivated()
{
	if (mIsVisible)
	{
		mSFXAppear->Play();
		Materialize();		
	}
	else
	{
		mSFXDisappear->Play();
		Unmaterialize();
	}

	return true;
}

AEnvHologram::AEnvHologram()
{
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a box that reacts to physics
	mBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = mBoxComponent;
	mBoxComponent->InitBoxExtent(mExtent);
	mBoxComponent->SetCollisionProfileName(TEXT("MovableEnv"));	
	mBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Create and position a mesh component so we can see where our box is
	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	mMeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (SphereVisualAsset.Succeeded())
	{
		mMeshComponent->SetStaticMesh(SphereVisualAsset.Object);
		auto meshExtent = mExtent;		
		mMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -meshExtent.Z));
		mMeshComponent->SetWorldScale3D(meshExtent / 50);
	}

	//static ConstructorHelpers::FObjectFinder<UMaterial> FadeMaterialAsset(TEXT("Material'/Game/Particle/FadeTemp.FadeTemp'"));
	//if (FadeMaterialAsset.Succeeded())
	//{
	//	mDynamicMaterialInstance = UMaterialInstanceDynamic::Create(FadeMaterialAsset.Object, mMeshComponent);
	//}
		
	mSFXAppear = CreateDefaultSubobject<USFXComponent>(TEXT("SFXAppear"));
	mSFXAppear->SetupAttachment(RootComponent);
	mSFXAppear->AddAudio("SoundWave'/Game/SFX/Platform_Appears/MMSB_OBJ_PLAT_APPEAR_003__1_.MMSB_OBJ_PLAT_APPEAR_003__1_'", USFXComponent::SFXAssetType::SoundWave);

	mSFXDisappear = CreateDefaultSubobject<USFXComponent>(TEXT("SFXDisappear"));
	mSFXDisappear->SetupAttachment(RootComponent);
	mSFXDisappear->AddAudio("SoundWave'/Game/SFX/Platform_disappears/MMSB_OBJ_PLAT_DISAPPEAR_003__1_.MMSB_OBJ_PLAT_DISAPPEAR_003__1_'", USFXComponent::SFXAssetType::SoundWave);
}

void AEnvHologram::Materialize()
{
	mBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	mMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	RootComponent->SetVisibility(true, true);

	//mDynamicMaterialInstance->SetScalarParameterValue("Fade", 1.0);
	//mMeshComponent->SetMaterial(0, mDynamicMaterialInstance);
}

void AEnvHologram::Unmaterialize()
{
	mBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent->SetVisibility(false, true);

	//mDynamicMaterialInstance->SetScalarParameterValue("Fade", 0.0);
	//mMeshComponent->SetMaterial(0, mDynamicMaterialInstance);
}

void AEnvHologram::OnConstruction(const FTransform & Transform)
{
	if (mIsVisible)
		Materialize();
	else
		Unmaterialize();
}

void AEnvHologram::Refresh()
{
	if (mIsVisible)
		Materialize();
	else
		Unmaterialize();
}



