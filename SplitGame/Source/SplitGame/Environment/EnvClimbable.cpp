// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvClimbable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SplitGame/SkeletonCharacter.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Component/SkeletonCharacterMovement.h"
#include "Engine/StaticMesh.h"
#include "Components/CapsuleComponent.h"
#include "Misc/OutputDeviceNull.h"

// Sets default values
AEnvClimbable::AEnvClimbable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a box that reacts to physics
	mBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	mBoxComponent->GetBodyInstance()->InertiaTensorScale = FVector(10000, 10000, 10000);
	SetRootComponent(mBoxComponent);
	//RootComponent = mBoxComponent;
	mBoxComponent->InitBoxExtent(mDefaultBoxExtend);
	//mBoxComponent->SetSimulatePhysics(true);
	mBoxComponent->SetEnableGravity(false);
	mBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//mBoxComponent->SetCollisionProfileName(TEXT("MovableEnv"));
	mBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//mBoxComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	mBoxComponent->SetLinearDamping(0);
	mBoxComponent->BodyInstance.bLockXRotation = true;
	mBoxComponent->BodyInstance.bLockYRotation = true;
	mBoxComponent->BodyInstance.bLockZRotation = true;
	mBoxComponent->BodyInstance.bLockXTranslation = true;
	mBoxComponent->BodyInstance.bLockYTranslation = true;

	// Create box collision and static mesh
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//RootComponent->SetRelativeLocation(FVector(0, 0, 0));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WallCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WallCollider"));
	WallCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	WallCollider->SetGenerateOverlapEvents(true);
	WallCollider->SetupAttachment(RootComponent);
	WallCollider->InitBoxExtent(mDefaultWallExtend);
	//WallCollider->SetWorldLocation(GetActorLocation());

	WallCollider->OnComponentBeginOverlap.AddDynamic(this, &AEnvClimbable::OnInteractiveBegin);
	WallCollider->OnComponentEndOverlap.AddDynamic(this, &AEnvClimbable::OnInteractiveEnd);
}

void AEnvClimbable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when the game starts or when spawned
void AEnvClimbable::BeginPlay()
{
	Super::BeginPlay();
	
}

//void AEnvClimbable::Refresh()
//{
//	//MeshComponent->SetStaticMesh(mMesh);
//
//}

void AEnvClimbable::Action(ASplitGameCharacter * Character)
{
}

void AEnvClimbable::OnInteractiveBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnInteractiveBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ASkeletonCharacter* character = Cast<ASkeletonCharacter>(OtherActor);
	if (character != nullptr && Cast<UCapsuleComponent>(OtherComp))
	{
		auto const direction = character->GetActorForwardVector();
		character->OnEnvInteractiveBegin(this);
		
		// attach if the charactering is jumping
		auto movement = character->GetCharacterMovement();
		if (movement->MovementMode == MOVE_Falling && !character->IsCarringSkin())
		{
			character->StartClimb();
		}
		else if (movement->MovementMode == MOVE_Custom && movement->CustomMovementMode == (uint8)ESkeletonCustomMoveMode::OnMuscle)
		{
			FOutputDeviceNull ar;
			character->CallFunctionByNameWithArguments(TEXT("EndFix"), ar, NULL, true);
			character->StartClimb();
		}
	}
}

void AEnvClimbable::OnInteractiveEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractiveEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	ASkeletonCharacter* character = Cast<ASkeletonCharacter>(OtherActor);
	if (character != nullptr && OtherComp == character->GetCapsuleComponent())
	{
		character->OnEnvInteractiveEnd(this);
		character->StopClimb();
	}

}
 
