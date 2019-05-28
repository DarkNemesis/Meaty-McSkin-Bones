// Fill out your copyright notice in the Description page of Project Settings.

#include "FallingBoxComponent.h"
#include <SplitGame/SkeletonCharacter.h>
#include <SplitGame/MuscleCharacter.h>
#include <SplitGame/SkinCharacter.h>
#include "Level/LevelManager.h"
#include "Engine/World.h"
#include "Environment/EnvAirVent.h"
#include <SplitGame/Component/FollowingCameraComponent.h>
#include "Components/CapsuleComponent.h"
#include "Component/SFXComponent.h"

UFallingBoxComponent::UFallingBoxComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	InitBoxExtent(FVector(50.0f, 50.0f, 10.0f));
	SetMobility(EComponentMobility::Movable);
	SetSimulatePhysics(false);
	SetEnableGravity(true);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);	
	SetCollisionResponseToAllChannels(ECR_Overlap);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);	

	m_followingCamera = CreateDefaultSubobject<UFollowingCameraComponent>(TEXT("FollowingCameraComponent"));

	mSFXThud = CreateDefaultSubobject<USFXComponent>(TEXT("SFXDisappear"));	
	mSFXThud->AddAudio("SoundWave'/Game/SFX/Box_Falls_and_Hits_Surface/MMSB_OBJ_BOOM_CHEESEHIT_005.MMSB_OBJ_BOOM_CHEESEHIT_005'", USFXComponent::SFXAssetType::SoundWave);
}

void UFallingBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* owner = GetOwner();
	m_ownerInitPosition = owner->GetActorLocation();

	ResetPosition();

	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	auto actorArray = levelManager->GetAllActors();
	for (int i = 0; i < actorArray.Num(); ++i)
	{
		auto actor = actorArray[i];
		if (Cast<ASkeletonCharacter>(actor) != nullptr)
		{
			actor->mOnCharacterReset.AddDynamic(this, &UFallingBoxComponent::Reset);
		}
	}

	OnComponentBeginOverlap.AddDynamic(this, &UFallingBoxComponent::OnOverlapBegin);
	OnComponentHit.AddDynamic(this, &UFallingBoxComponent::OnColliderHit);

	isBoxActive = false;
}

void UFallingBoxComponent::DestorySelf()
{
	DestroyComponent();
}

void UFallingBoxComponent::ResetPosition()
{
	auto* owner = GetOwner();
	UBoxComponent* ownerBox = Cast<UBoxComponent>(owner->GetComponentByClass(UBoxComponent::StaticClass()));
	if (ownerBox != nullptr)
	{
		auto ownerExtents = ownerBox->GetUnscaledBoxExtent();
		auto myExtents = GetUnscaledBoxExtent();
		SetRelativeLocation(FVector(0, 0, -(ownerExtents.Z + myExtents.Z)));
	}
}

void UFallingBoxComponent::SetBoxActive(bool i_active)
{
	isBoxActive = i_active;
}



void UFallingBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	//Check if there's a skeleton below
	FVector startPoint = GetOwner()->GetActorLocation();
	FVector Direc = FVector(0, 0, -1.0f);
	FVector endPoint = startPoint + Direc * 2000.0f;
	FVector extents = FVector(120.0f, 120.0f, 120.0f);
	FHitResult Hit(ForceInit);
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	bool hasHit = GetWorld()->SweepSingleByChannel(Hit, startPoint, endPoint, Direc.ToOrientationQuat(), ECC_WorldDynamic, FCollisionShape::MakeBox(extents), params); // simple trace function
	isSkeletonBelow = (hasHit && Cast<ASkeletonCharacter>(Hit.Actor) != nullptr);

}

void UFallingBoxComponent::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto actor = Cast<ASkeletonCharacter>(OtherActor);
	if (isBoxActive && actor != nullptr && !actor->IsDead() && OtherComp->IsA(UCapsuleComponent::StaticClass()))
	{
		actor->SetDead(true);
		shouldOwnerRespawn = true;
	}
	if (!isOwnerAttachedByRope)
	{		
		if (Cast<ASplitGameCharacter>(OtherActor))
			return;
		if (OtherComp->GetFName() == AEnvAirVent::mAirColumnColliderFName)
			return;
		mSFXThud->Play();
	}
}

void UFallingBoxComponent::OnColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

UFollowingCameraComponent * UFallingBoxComponent::GetFollowingCamera()
{
	return m_followingCamera;
}

void UFallingBoxComponent::Reset(ASplitGameCharacter* i_character)
{
	if (Cast<ASkeletonCharacter>(i_character) != nullptr)
	{
		if (isOwnerAttachedByRope)
		{
			m_onSkeletonKilled.Broadcast();
		}
		//else
		//{
		//	FVector offset = FVector(0, 0, 10.0f);
		//	bool flag = false;
		//	FHitResult result;
		//	GetOwner()->SetActorLocation(m_ownerInitPosition + offset, false, &result, ETeleportType::None);
		//}
	}
}