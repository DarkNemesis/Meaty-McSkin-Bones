// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvElevator.h"
#include "Component/SFXComponent.h"

#include <Runtime/Engine/Classes/Engine/World.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SceneComponent.h>

AEnvElevator::AEnvElevator()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create box collision and static mesh
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetRelativeLocation(FVector(0, 0, 0));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	for (int i = 0; i < 4; i++)
	{
		FName frameName(TEXT("DoorFrame"), i);
		UStaticMeshComponent* frameComponent = CreateDefaultSubobject<UStaticMeshComponent>(frameName);
		frameComponent->SetupAttachment(RootComponent);
		switch (i)
		{
		case 0:
			frameComponent->SetRelativeLocation(FVector(0, -mDefaultExtend.Y, mDefaultExtend.Z));
			break;
		case 1:
			frameComponent->SetRelativeLocation(FVector(0, mDefaultExtend.Y, mDefaultExtend.Z));
			break;
		case 2:
			frameComponent->SetRelativeLocation(FVector(0, -mDefaultExtend.Y, -mDefaultExtend.Z));
			break;
		case 3:
			frameComponent->SetRelativeLocation(FVector(0, mDefaultExtend.Y, -mDefaultExtend.Z));
			break;
		default:
			break;
		}
		frameComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mDoorFrameComponents.Add(frameComponent);

		FName doorName(TEXT("Door"), i );
		UStaticMeshComponent* doorComponent = CreateDefaultSubobject<UStaticMeshComponent>(doorName);
		doorComponent->SetupAttachment(frameComponent);
		mDoorMeshComponents.Add(doorComponent);
	}

	mTimerDelegate.BindUFunction(this, "Move");

	mActiveAudio = CreateDefaultSubobject<USFXComponent>(TEXT("ActiveAudio"));
	mActiveAudio->SetupAttachment(RootComponent);
	mActiveAudio->AddAudio("SoundWave'/Game/SFX/Interactable_Button/MMSB_MENU_MAIN_SELECT_001__2_.MMSB_MENU_MAIN_SELECT_001__2_'", USFXComponent::SoundWave);

	mDoorSupport = mDefaultDoorSupport;
}

void AEnvElevator::BeginPlay()
{	
	Super::BeginPlay();

	mOffset = (bStartFromBottom ? FVector(0, 0, 1) * mExtend.Z : FVector(0, 0, -1) * mExtend.Z);
	mDirection = mOffset.GetSafeNormal();

	if (bStartFromBottom)
	{
		mDoorMeshComponents[2]->SetRelativeLocation(FVector(mDoorWidth, 0, 0));
		mDoorMeshComponents[3]->SetRelativeLocation(FVector(mDoorWidth, 0, 0));
	}
	else
	{
		mDoorMeshComponents[0]->SetRelativeLocation(FVector(mDoorWidth, 0, 0));
		mDoorMeshComponents[1]->SetRelativeLocation(FVector(mDoorWidth, 0, 0));
	}

	GetAttachedActors(mAttachedActors);
}

bool AEnvElevator::KnobActivated()
{
	if (GetWorld())
	{
		FTimerManager& timeManager = GetWorld()->GetTimerManager();
		if (!timeManager.TimerExists(mTimerHandle))
		{
			timeManager.SetTimer(mTimerHandle, mTimerDelegate, 0.001f, true, 0);
			mActiveAudio->Play();
			// set door
			if (mOffset.Z > 0)
			{
				ActivateDoor(true, false);
			}
			else
			{
				ActivateDoor(false, false);
			}
			return true;
		}
	}

	return false;
}

void AEnvElevator::Refresh()
{
	if (mDoorSupport & (1 << (uint8)EElevatorDoorBits::TopLeft))
	{
		mDoorFrameComponents[0]->SetVisibility(true, true);
		mDoorMeshComponents[0]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		mDoorFrameComponents[0]->SetVisibility(false, true);
		mDoorMeshComponents[0]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (mDoorSupport & (1 << (uint8)EElevatorDoorBits::TopRight))
	{
		mDoorFrameComponents[1]->SetVisibility(true, true);
		mDoorMeshComponents[1]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		mDoorFrameComponents[1]->SetVisibility(false, true);
		mDoorMeshComponents[1]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (mDoorSupport & (1 << (uint8)EElevatorDoorBits::BottomLeft))
	{
		mDoorFrameComponents[2]->SetVisibility(true, true);
		mDoorMeshComponents[2]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		mDoorFrameComponents[2]->SetVisibility(false, true);
		mDoorMeshComponents[2]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (mDoorSupport & (1 << (uint8)EElevatorDoorBits::BottomRight))
	{
		mDoorFrameComponents[3]->SetVisibility(true, true);
		mDoorMeshComponents[3]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		mDoorFrameComponents[3]->SetVisibility(false, true);
		mDoorMeshComponents[3]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (mExtend != mDefaultExtend)
	{
		mDoorFrameComponents[0]->SetRelativeLocation(FVector(0, -mExtend.Y, mExtend.Z));
		mDoorFrameComponents[1]->SetRelativeLocation(FVector(0, mExtend.Y, mExtend.Z));
		mDoorFrameComponents[2]->SetRelativeLocation(FVector(0, -mExtend.Y, -mExtend.Z));
		mDoorFrameComponents[3]->SetRelativeLocation(FVector(0, mExtend.Y, -mExtend.Z));
	}

	for (int i = 0; i < mDoorFrameComponents.Num(); i++)
	{
		mDoorFrameComponents[i]->SetStaticMesh(mDoorFrameMesh);
		mDoorMeshComponents[i]->SetStaticMesh(mDoorMesh);
	}

	if (bStartFromBottom)
	{
		MeshComponent->SetRelativeLocation(FVector(0, 0, -mExtend.Z));
	}
	else
	{
		MeshComponent->SetRelativeLocation(FVector(0, 0, mExtend.Z));
	}
}

void AEnvElevator::Move()
{
	if (GetWorld())
	{
		auto& timeManager = GetWorld()->GetTimerManager();
		float deltaTime = timeManager.GetTimerRate(mTimerHandle);
		FVector offset = mSpeed * mDirection * deltaTime;

		// finish
		if ( FVector::DotProduct(MeshComponent->GetRelativeTransform().GetLocation() - mOffset, mDirection) >= 0)
		{
			MeshComponent->SetRelativeLocation(mOffset);
			//for (auto actor : mAttachedActors)
			//{
			//	actor->AddActorWorldOffset(MeshComponent->GetRelativeTransform().GetLocation() - mOffset, true, nullptr, ETeleportType::TeleportPhysics);
			//}

			if (mOffset.Z > 0)
			{
				ActivateDoor(false, true);
			}
			else
			{
				ActivateDoor(true, true);
			}
			mOffset = -mOffset;
			mDirection = mOffset.GetSafeNormal();
			timeManager.ClearTimer(mTimerHandle);
		}
		else
		{
			MeshComponent->AddLocalOffset(offset);
			//for (auto actor : mAttachedActors)
			//{
			//	actor->AddActorWorldOffset(offset, true, nullptr, ETeleportType::TeleportPhysics);
			//}
		}
	}
}

void AEnvElevator::ActivateDoor(bool bBottom, bool bOpen)
{
	if (GetWorldTimerManager().TimerExists(mDoorTimeHandle))
	{
		GetWorldTimerManager().ClearTimer(mDoorTimeHandle);
	}
	mDoorDelegate.BindUFunction(this, "MoveDoor", bBottom, bOpen);
	GetWorldTimerManager().SetTimer(mDoorTimeHandle, mDoorDelegate, 0.01f, true, 0);
}

void AEnvElevator::MoveDoor(bool bBottom, bool bOpen)
{
	if (bBottom)
	{
		mDoorMeshComponents[2]->AddLocalOffset(FVector((bOpen ? 1 : -1) * mDoorWidth * GetWorldTimerManager().GetTimerRate(mDoorTimeHandle), 0, 0));
		mDoorMeshComponents[3]->AddLocalOffset(FVector((bOpen ? 1 : -1) * mDoorWidth * GetWorldTimerManager().GetTimerRate(mDoorTimeHandle), 0, 0));
		
		float x = mDoorMeshComponents[2]->GetRelativeTransform().GetLocation().X;
		if (x < 0 || x > mDoorWidth)
		{
			mDoorMeshComponents[2]->SetRelativeLocation(FVector(bOpen ? mDoorWidth : 0, 0, 0));
			mDoorMeshComponents[3]->SetRelativeLocation(FVector(bOpen ? mDoorWidth : 0, 0, 0));
			GetWorldTimerManager().ClearTimer(mDoorTimeHandle);
		}
	}
	else
	{
		mDoorMeshComponents[0]->AddLocalOffset(FVector((bOpen ? 1 : -1) * mDoorWidth * GetWorldTimerManager().GetTimerRate(mDoorTimeHandle), 0, 0));
		mDoorMeshComponents[1]->AddLocalOffset(FVector((bOpen ? 1 : -1) * mDoorWidth * GetWorldTimerManager().GetTimerRate(mDoorTimeHandle), 0, 0));

		float x = mDoorMeshComponents[0]->GetRelativeTransform().GetLocation().X;
		if (x < 0 || x > mDoorWidth)
		{
			mDoorMeshComponents[0]->SetRelativeLocation(FVector(bOpen ? mDoorWidth : 0, 0, 0));
			mDoorMeshComponents[1]->SetRelativeLocation(FVector(bOpen ? mDoorWidth : 0, 0, 0));
			GetWorldTimerManager().ClearTimer(mDoorTimeHandle);
		}
	}


}

