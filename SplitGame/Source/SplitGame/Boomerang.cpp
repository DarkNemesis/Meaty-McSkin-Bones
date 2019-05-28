// Fill out your copyright notice in the Description page of Project Settings.

#include "Boomerang.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/World.h"
#include "Component/SFXComponent.h"
#include "Engine/StaticMesh.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ABoomerang::ABoomerang()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxRange = 500.0f;
	InitFlyingSpeed = 1500.0f;
	IsFirstOverlapWithCharacter = false;
	IsFirstHit = false;
	Duration = 3.0f;
	m_flyingForwardDuration = 1.0f;
	BounceDuration = 0.2f;
	BackForce = 1500.0f;
	Radius = 30.0f;

	 //Create and position a mesh component so we can see where our box is


	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoomerangCollision"));
	RootComponent = CollisionComponent;

	FVector extents(Radius, Radius, Radius);
	CollisionComponent->InitBoxExtent(extents);
	CollisionComponent->SetCollisionProfileName(TEXT("Boomerang"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	//CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABoomerang::OnBoomerangOverlap);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABoomerang::OnBoomerangHit);

	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetEnableGravity(false);

	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	mMeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("StaticMesh'/Game/Mannequin/Character/Mesh/ArmBoomerang'"));
	if (SphereVisualAsset.Succeeded())
	{
		mMeshComponent->SetStaticMesh(SphereVisualAsset.Object);
		mMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mMeshComponent->SetSimulatePhysics(false);
		mMeshComponent->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));
		mMeshComponent->SetMobility(EComponentMobility::Movable);
	}

	mSFXShoot = CreateDefaultSubobject<USFXComponent>(TEXT("ShootAudio"));
	mSFXShoot->SetupAttachment(RootComponent);
	mSFXShoot->AddAudio("SoundWave'/Game/SFX/Bone_Throw_VO/Edited_bone_throw1.Edited_bone_throw1'", USFXComponent::SFXAssetType::SoundWave);
	mSFXShoot->AddAudio("SoundWave'/Game/SFX/Bone_Throw_VO/edited_bone_throw2.edited_bone_throw2'", USFXComponent::SFXAssetType::SoundWave);

	mSFXFlying = CreateDefaultSubobject<USFXComponent>(TEXT("FlyAudio"));
	mSFXFlying->SetupAttachment(RootComponent);
	mSFXFlying->AddAudio("SoundWave'/Game/SFX/Bonemerang_Mid_Air/MMSB_CHR_BONE_BOOMAIR_003.MMSB_CHR_BONE_BOOMAIR_003'", USFXComponent::SFXAssetType::SoundWave);

	mSFXHit = CreateDefaultSubobject<USFXComponent>(TEXT("HitAudio"));
	mSFXHit->SetupAttachment(RootComponent);
	mSFXHit->AddAudio("SoundWave'/Game/SFX/Bonemerang_hits_hard_surface/MMSB_OBJ_BONE_HITS_HARD_SURFACE_001___1_.MMSB_OBJ_BONE_HITS_HARD_SURFACE_001___1_'", USFXComponent::SFXAssetType::SoundWave);

	mSFXCutRope = CreateDefaultSubobject<USFXComponent>(TEXT("CutRopeAudio"));
	mSFXCutRope->SetupAttachment(RootComponent);
	mSFXCutRope->AddAudio("SoundWave'/Game/SFX/Rope_Cut/new_rope_cut.new_rope_cut'", USFXComponent::SFXAssetType::SoundWave);
}

// Called when the game starts or when spawned
void ABoomerang::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->SetMassOverrideInKg(NAME_None, 1.0f, true);
	CollisionComponent->SetAllPhysicsLinearVelocity(GetActorForwardVector() * InitFlyingSpeed, false);

	auto pos = GetActorLocation();
	auto rot = GetActorRotation();

	auto v = CollisionComponent->ComponentVelocity;
	m_FlyingState = ABoomerang::Flyforward;
	m_FlyForwardTimer = m_flyingForwardDuration;

	mSFXShoot->Play();
	mSFXFlying->Play();
}

void ABoomerang::SwitchFlyState(FlyState i_state)
{
	m_FlyingState = i_state;
}

void ABoomerang::OnFlyForwardUpdate(float DeltaTime)
{
	FVector force = GetActorForwardVector() * (-BackForce);
	force.Z = 0;
	force.X = 0;
	if (CollisionComponent != nullptr)
	{
		CollisionComponent->AddForce(force);
	}
	
}

void ABoomerang::OnBounceBackUpdate(float DeltaTime)
{	
	FVector upForce(0, 0, -4000.0f);
	CollisionComponent->AddForce(upForce);
}

void ABoomerang::OnFlyBackUpdate(float DeltaTime)
{
	auto ownerPosition = m_owner->GetActorTransform().GetLocation();
	FVector dir = (ownerPosition - GetActorLocation()).GetSafeNormal();
	CollisionComponent->SetPhysicsLinearVelocity(dir * 1500.0f);
}

// Called every frame
void ABoomerang::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	mMeshComponent->AddWorldRotation(FRotator(0, 45.0f, 0));

	switch (m_FlyingState)
	{
	case ABoomerang::Flyforward:
		OnFlyForwardUpdate(DeltaTime);
		break;
	case ABoomerang::BounceBack:
		OnBounceBackUpdate(DeltaTime);
		break;
	case ABoomerang::FlyBack:
		OnFlyBackUpdate(DeltaTime);
		break;
	default:
		break;
	}	

	if (m_FlyForwardTimer > 0)
	{
		m_FlyForwardTimer -= DeltaTime;
		if (m_FlyForwardTimer <= 0)
		{
			SwitchFlyState(FlyBack);
		}
	}

	if (m_BounceTimer > 0)
	{
		m_BounceTimer -= DeltaTime;
		if (m_BounceTimer <= 0)
		{
			SwitchFlyState(FlyBack);
		}
	}

	if (Duration > 0)
	{
		Duration -= DeltaTime;
		if (Duration <= 0)
		{
			Destroy();
			//m_owner->isBoomerangReady = true;
			m_owner->SetBoomerangAnimationType(ESkeletonBoomerangAnimation::Catch);

		}
	}

}

void ABoomerang::OnBoomerangOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & Hit)
{
	if (m_FlyingState == FlyBack && Cast<ASkeletonCharacter>(OtherActor) != nullptr && (OverlappedComponent->GetFName() == ACharacter::CapsuleComponentName || OtherComponent->GetFName() == ACharacter::CapsuleComponentName))
	{
		Catch();
		mSFXFlying->Stop();
	}
}


void ABoomerang::OnBoomerangHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	SwitchFlyState(BounceBack);
	m_BounceTimer = BounceDuration;
	auto velocity = CollisionComponent->GetPhysicsLinearVelocity() + FVector(0, 0, 800.0f);
	CollisionComponent->SetPhysicsLinearVelocity(velocity);
	m_FlyForwardTimer = -1.0f;

	//If hits any objects, the boomerang won't hit anything anymore
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	mSFXHit->Play();
}

void ABoomerang::SetInitLocation(FVector i_pos)
{
	m_InitLocation = i_pos;
}

void ABoomerang::SetInitRotation(FRotator i_rot)
{
	m_InitRotation = i_rot;
}

void ABoomerang::SetInitTransform(FTransform trans)
{
	//SetActorTransform(trans);
	m_InitTransform = trans;
}

void ABoomerang::Initialize()
{
	SetActorLocation(m_InitLocation);
	SetActorRotation(m_InitRotation);
}

ASkeletonCharacter* ABoomerang::m_owner = nullptr;

void ABoomerang::Shoot(ASplitGameCharacter * character)
{
	character->ForceHorizontalRotate();
	auto pos = character->GetActorLocation();
	auto rot = (-1.0f) * character->GetActorRotation();


	auto* world = character->GetWorld();
	if (world != nullptr)
	{
		auto boomerang = world->SpawnActor<ABoomerang>(pos, rot);
		if (Cast<ASkeletonCharacter>(character) != nullptr)
			boomerang->IsShotBySkeleton = true;
		else
			boomerang->IsShotBySkeleton = false;
	}

	
}

void ABoomerang::Catch()
{
	//m_owner->isBoomerangReady = true;
	m_owner->SetBoomerangAnimationType(ESkeletonBoomerangAnimation::Catch);
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABoomerang::DestoryBoomerang, 0.2f, false);	
}

void ABoomerang::DestoryBoomerang()
{
	Destroy();
}

void ABoomerang::PlayCutRopeSFX()
{
	mSFXCutRope->Play();
}

void ABoomerang::SetBoomerangOwner(ASkeletonCharacter * i_owner)
{
	m_owner = i_owner;
}
