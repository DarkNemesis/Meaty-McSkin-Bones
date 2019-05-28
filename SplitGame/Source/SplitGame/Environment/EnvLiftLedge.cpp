// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvLiftLedge.h"
#include "MuscleCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
AEnvLiftLedge::AEnvLiftLedge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a box that reacts to physics
	UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Root Component"));
	RootComponent = BoxComponent;
	BoxComponent->InitBoxExtent(mExtent);
	//BoxComponent->SetSimulatePhysics(true);
	BoxComponent->SetCollisionProfileName(TEXT("ENVLiftLedge"));
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Create and position a mesh component so we can see where our box is
	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	mMeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (SphereVisualAsset.Succeeded())
	{
		mMeshComponent->SetStaticMesh(SphereVisualAsset.Object);
		mMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -mExtent.Z));
		mMeshComponent->SetWorldScale3D(mExtent / 50);
	}

	mHandle.X = mExtent.X;
	mBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Component"));
	mBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mBoxComponent->SetupAttachment(RootComponent);
	mBoxComponent->InitBoxExtent(mHandle);
	mBoxComponent->SetRelativeLocation(FVector(0.f, 0.f, mExtent.Z));
	mBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvLiftLedge::OnInteractiveBegin);
	mBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvLiftLedge::OnInteractiveEnd);

	if (mDestinationZ == 0)
		mDestinationZ = RootComponent->GetComponentLocation().Y;

	mInitialZ = RootComponent->GetComponentLocation().Z;
}

void AEnvLiftLedge::Refresh()
{
	if (mExtent == mDefaultExtent)
		return;

	((UBoxComponent*)RootComponent)->InitBoxExtent(mExtent);

	mMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -mExtent.Z));
	mMeshComponent->SetWorldScale3D(mExtent / 50);

	mHandle.X = mExtent.X;
	mBoxComponent->SetBoxExtent(mHandle);
	mBoxComponent->SetRelativeLocation(FVector(0.f, 0.f, mExtent.Z));

	mInitialZ = RootComponent->GetComponentLocation().Z;
}

void AEnvLiftLedge::OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (Cast<AMuscleCharacter>(OtherActor))
	{
		Cast<AMuscleCharacter>(OtherActor)->OnEnvInteractiveBegin(this);
	}
}

void AEnvLiftLedge::OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AMuscleCharacter>(OtherActor))
	{
		Cast<AMuscleCharacter>(OtherActor)->OnEnvInteractiveEnd(this);
	}
}

void AEnvLiftLedge::Action(ASplitGameCharacter* Character)
{
	auto location = RootComponent->GetComponentLocation();

	if (location.Z == mDestinationZ)
		return;
	if (location.Z < mDestinationZ)
	{
		location.Z = FMath::Min(location.Z + mRiseSpeed, mDestinationZ);
	}
	else if (location.Z > mDestinationZ)
	{
		location.Z = FMath::Max(location.Z - mRiseSpeed, mDestinationZ);
	}
	
	RootComponent->SetWorldLocation(location);

	bIsBeingHeld = true;
}

void AEnvLiftLedge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsBeingHeld)
	{
		auto location = RootComponent->GetComponentLocation();

		if (location.Z == mInitialZ)
			return;
		if (location.Z < mInitialZ)
		{
			location.Z = FMath::Min(location.Z + mFallSpeed, mInitialZ);
		}
		else if (location.Z > mInitialZ)
		{
			location.Z = FMath::Max(location.Z - mFallSpeed, mInitialZ);
		}

		RootComponent->SetWorldLocation(location);
	}

	bIsBeingHeld = false;
 }


