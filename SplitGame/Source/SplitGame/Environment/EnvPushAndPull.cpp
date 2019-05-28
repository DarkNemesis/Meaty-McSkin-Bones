#include "EnvPushAndPull.h"
#include "MuscleCharacter.h"
#include "Engine/StaticMesh.h"
#include "Component/SFXComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

AEnvPushAndPull::AEnvPushAndPull()
{
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a box that reacts to physics
	mBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = mBoxComponent;
	mBoxComponent->InitBoxExtent(mExtent);
	mBoxComponent->SetSimulatePhysics(true);
	mBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	mBoxComponent->SetCollisionProfileName(TEXT("MovableEnv"));
	mBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	mBoxComponent->SetLinearDamping(0);
	//mBoxComponent->GetBodyInstance()->InertiaTensorScale = FVector(10000, 10000, 10000);

	mBoxComponent->BodyInstance.bLockXRotation = true;
	mBoxComponent->BodyInstance.bLockYRotation = true;
	mBoxComponent->BodyInstance.bLockZRotation = true;
	mBoxComponent->BodyInstance.bLockXTranslation = true;

	// Create and position a mesh component so we can see where our box is
	if (mStaticMesh)
	{
		mMeshComponent->SetStaticMesh(mStaticMesh);
	}
	else
	{
		mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
		mMeshComponent->SetupAttachment(RootComponent);
		mMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
		if (SphereVisualAsset.Succeeded())
		{
			mMeshComponent->SetStaticMesh(SphereVisualAsset.Object);
		}
	}	
	
	mTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	mTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mTriggerComponent->SetupAttachment(RootComponent);
	mTriggerComponent->InitBoxExtent(FVector(mExtent.X, mExtent.Y + 130, mExtent.Z + 10));
	mTriggerComponent->SetRelativeLocation(FVector(0, 0, -10));
	mTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvPushAndPull::OnInteractiveBegin);
	mTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvPushAndPull::OnInteractiveEnd);

	mFallingBoxComponent = CreateDefaultSubobject<UFallingBoxComponent>(TEXT("FallingBoxComponent"));
	mFallingBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mFallingBoxComponent->SetupAttachment(RootComponent);
	mFallingBoxExtents = FVector(mExtent.X, mExtent.Y, 10.0f);
	mFallingBoxComponent->InitBoxExtent(mFallingBoxExtents);
	mFallingBoxComponent->SetRelativeLocation(FVector(0, 0, -(mExtent.Z + mFallingBoxExtents.Z)));
	
	mSFXSliding = CreateDefaultSubobject<USFXComponent>(TEXT("SlidingAudio"));
	mSFXSliding->SetupAttachment(RootComponent);
	mSFXSliding->AddAudio("SoundWave'/Game/SFX/Box_Slide/Edited_Cheese_Box.Edited_Cheese_Box'", USFXComponent::SFXAssetType::SoundWave);
}

void AEnvPushAndPull::Refresh()
{
	if (mStaticMesh)
	{
		mMeshComponent->SetStaticMesh(mStaticMesh);
	}

	((UBoxComponent*)RootComponent)->InitBoxExtent(mExtent);

	mTriggerComponent->SetBoxExtent(FVector(mExtent.X, mExtent.Y + 130, mExtent.Z + 10));
	mTriggerComponent->SetRelativeLocation(FVector(0, 0, -10));

	mFallingBoxExtents = FVector(mExtent.X, mExtent.Y, 10.0f);
	mFallingBoxComponent->InitBoxExtent(mFallingBoxExtents);
	mFallingBoxComponent->SetRelativeLocation(FVector(0, 0, -(mExtent.Z + mFallingBoxExtents.Z)));

}
void AEnvPushAndPull::OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
 	if (Cast<AMuscleCharacter>(OtherActor) && !IsAttachedByRope)
	{
		Cast<AMuscleCharacter>(OtherActor)->OnEnvInteractiveBegin(this);
		bIsAwake = 1;
		mBoxComponent->SetSimulatePhysics(true);
	}
	//else if (Cast<AEnvObject>(OtherActor))
	//{
	//	bIsAwake = 1;
	//	mBoxComponent->SetSimulatePhysics(true);
	//}
}

void AEnvPushAndPull::OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AMuscleCharacter>(OtherActor) && !IsAttachedByRope)
	{
		Cast<AMuscleCharacter>(OtherActor)->OnEnvInteractiveEnd(this);
		bIsAwake = 1;
		mBoxComponent->SetSimulatePhysics(true);
	}
	//else if (Cast<AEnvObject>(OtherActor))
	//{
	//	bIsAwake = 1;
	//	mBoxComponent->SetSimulatePhysics(true);
	//}
}

void AEnvPushAndPull::BeginPlay()
{	
	AEnvObject::BeginPlay();

	TArray<AActor*> child;
	GetAttachedActors(child);
	for (auto& itr : child)
	{			
		((UBoxComponent*)(itr->GetRootComponent()))->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnvPushAndPull::Action(ASplitGameCharacter* Character)
{
	bIsAwake = 1;
	mBoxComponent->SetSimulatePhysics(true);
	
	auto objectLocation = RootComponent->GetComponentLocation();
	auto characterLocation = Character->GetActorLocation();

	FVector charOrigin;
	FVector charExtent;
	Character->GetActorBounds(true, charOrigin, charExtent);

	float padding = -35;

	if (objectLocation.Y < characterLocation.Y)
		objectLocation.Y = (characterLocation.Y - charExtent.Y) - mExtent.Y  - padding;
	else
		objectLocation.Y = (characterLocation.Y + charExtent.Y) + mExtent.Y  + padding;
		
	auto objectVelocity = ((UBoxComponent*)RootComponent)->GetPhysicsLinearVelocity();
	objectVelocity.Y = Character->GetInputAxisValue("Walk") * 250;
	objectVelocity.Z = FMath::Min(0.f, objectVelocity.Z);
	((UBoxComponent*)RootComponent)->SetPhysicsLinearVelocity(objectVelocity);
		   
	SetActorLocation(objectLocation, true, nullptr, ETeleportType::None);
	
	if (Character->GetInputAxisValue("Walk") != 0)
		mSFXSliding->Play();
	else
		mSFXSliding->Stop();
}

void AEnvPushAndPull::Tick(float DeltaTime)
{
	AEnvObject::Tick(DeltaTime);
	SetActorRotation(FRotator(0, 0, 0), ETeleportType::TeleportPhysics);

	if (bIsAwake)
	{
		if (bIsAwake < 10)
			bIsAwake++;
		else if (bIsAwake >= 10)
		{
			mSFXSliding->Stop();
			if (((UBoxComponent*)RootComponent)->GetPhysicsLinearVelocity().Size() <= 1)
			{
				mBoxComponent->SetSimulatePhysics(false);
				bIsAwake = 0;
			}
		}		
	}	
}