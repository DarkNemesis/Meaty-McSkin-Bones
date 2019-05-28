// Fill out your copyright notice in the Description page of Project Settings.

#include "SkinCatapultComponent.h"
#include "SkeletonCharacter.h"
#include "SkinCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h" 
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values for this component's properties
USkinCatapultComponent::USkinCatapultComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;	

	mProjectileTrajectory = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileTrajectory"));
	mProjectileTrajectory->bAutoActivate = true;
	mProjectileTrajectory->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/VFX/Beam/Beam.Beam'"));
	if (ParticleAsset.Succeeded())
	{
		mProjectileTrajectory->SetTemplate(ParticleAsset.Object);
	}
	mProjectileParticle = ParticleAsset.Object;
}

void USkinCatapultComponent::SetParameters(ASkinCharacter* Skin, ASkeletonCharacter* Skeleton)
{
	mPointOfOrigin = Skeleton->GetActorLocation();
	mSkeletonCharacter = Skeleton;
	mSkinCharacter = Skin;

	if (Skin->GetActorLocation().Y <= mPointOfOrigin.Y)
		mDirection = -1;
	else
		mDirection = 1;

	mMaxPointOfAim = mPointOfOrigin;
	mMaxPointOfAim.Y = mPointOfOrigin.Y + (mDirection * mMaxAimableDistance);

	mMinPointOfAim = mPointOfOrigin;
	mMinPointOfAim.Y = mPointOfOrigin.Y + (mDirection * mMinAimableDistance);
	
	mProjectileTrajectory->SetupAttachment(GetOwner()->GetRootComponent());
}

// Called when the game starts
void USkinCatapultComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USkinCatapultComponent::KeepInBounds()
{
	float left = FMath::Min(mMaxPointOfAim.Y, mMinPointOfAim.Y);
	float right = FMath::Max(mMaxPointOfAim.Y, mMinPointOfAim.Y);

	FVector curr = mSkinCharacter->GetActorLocation();

	curr.Y = FMath::Max(left, FMath::Min(curr.Y, right));
	mSkinCharacter->SetActorLocation(curr);
}

void USkinCatapultComponent::Launch()
{	
	mSkinCharacter->GetCharacterMovement()->AddImpulse(GetProjectileDirection() * mLaunchVelocity, true);

	for (auto& itr : BeamArray)
		itr->DestroyComponent();

	BeamArray.Empty();
}

FVector USkinCatapultComponent::GetProjectileDirection()
{
	FVector skelOrigin;
	FVector skelExtent;
	mSkeletonCharacter->GetActorBounds(true, skelOrigin, skelExtent);

	FVector skinOrigin;
	FVector skinExtent;
	mSkinCharacter->GetActorBounds(true, skinOrigin, skinExtent);

	FVector dirEnd = skelOrigin;
	dirEnd.Z += (skelExtent.Z);

	FVector dirStart = skinOrigin;
	dirStart.Z -= (skinExtent.Y);

	auto velocity = (dirEnd - dirStart);
	velocity.Normalize();

	return velocity;
}

void USkinCatapultComponent::DestroyComponent(bool bPromoteChildren)
{
	for (auto& itr : BeamArray)
		itr->DestroyComponent();

	BeamArray.Empty();

	Super::DestroyComponent(bPromoteChildren);
}


void USkinCatapultComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	KeepInBounds();		

	DrawProjectileTrajectory();
}

void GetSegmentAtTime(FVector i_startLocation, FVector i_initialVelocity, FVector i_Gravity, float time1, float time2,
	FVector &o_point1, FVector &o_point2)
{
	const auto offset1 = i_initialVelocity * time1 + time1 * time1 * i_Gravity / 2;
	o_point1 = i_startLocation + offset1;

	const auto offset2 = i_initialVelocity * time2 + time2 * time2 * i_Gravity / 2;
	o_point2 = i_startLocation + offset2;
}

void USkinCatapultComponent::DrawProjectileTrajectory()
{	
	for (auto& itr : BeamArray)
		itr->DestroyComponent();

	BeamArray.Empty();

	// Path Duration
	float _mPathLifeTime = 5.0f;
	// Time between each point 
	float _mTimeInterval = 0.05f;
	const FVector _mGravity = FVector(0, 0, -980 * 2.f);

	auto i_direction = GetProjectileDirection();
	auto i_startPoint = mSkinCharacter->GetActorLocation();

	auto _mEndPoint = i_startPoint + i_direction * mLaunchVelocity;
	auto _mInitialVelocity = i_direction * mLaunchVelocity;
	const auto pointCount = _mPathLifeTime / _mTimeInterval;

	for (auto i = 0; i < pointCount; i++)
	{
		// Calculate segment points
		auto timeStep = i * _mTimeInterval;
		auto nextTimeStep = (i + 1) * _mTimeInterval;

		FVector currentStepPoint;
		FVector nextStepPoint;
		GetSegmentAtTime(i_startPoint, _mInitialVelocity, _mGravity, timeStep, nextTimeStep, currentStepPoint, nextStepPoint);


		// line trace
		FHitResult outHit;
		FCollisionQueryParams collision_query_params;

		bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, currentStepPoint, nextStepPoint, ECC_Visibility);
			   
		mProjectileTrajectory = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mProjectileParticle, currentStepPoint, FRotator::ZeroRotator, true);
		BeamArray.Add(mProjectileTrajectory);

		mProjectileTrajectory->SetBeamSourcePoint(0, currentStepPoint, 0);
		mProjectileTrajectory->SetBeamTargetPoint(0, nextStepPoint, 0);
	}
}

