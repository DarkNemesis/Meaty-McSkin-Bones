#include "EnvKnob.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "SplitGameCharacter.h"
#include "SplitGame/Boomerang.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Level/LevelManager.h"
#include <SplitGame/Component/FollowingCameraComponent.h>
#include "EnvKnobInterface.h"

AEnvKnob::AEnvKnob()
{
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a box that reacts to physics
	UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = BoxComponent;
	BoxComponent->InitBoxExtent(mExtent);
	BoxComponent->SetCollisionProfileName(TEXT("MovableEnv"));
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	
	// Create and position a mesh component so we can see where our box is
	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	mMeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (SphereVisualAsset.Succeeded())
	{
		mMeshComponent->SetStaticMesh(SphereVisualAsset.Object);
		FVector meshExtent = FVector(mExtent.X, mExtent.Y, mExtent.Z + mVisualHeight);
		mMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -meshExtent.Z));
		mMeshComponent->SetWorldScale3D(meshExtent / 50);
	}

	mBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	mBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mBoxComponent->SetupAttachment(RootComponent);	
	mBoxComponent->InitBoxExtent(FVector(mExtent.X, mExtent.Y, mExtent.Z + mCollisionHeight));
	mBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvKnob::OnInteractiveBegin);
	mBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvKnob::OnInteractiveEnd);

	m_followingCamera = CreateDefaultSubobject<UFollowingCameraComponent>(TEXT("FollowingCameraComponent"));
	m_followingCamera->SetupAttachment(RootComponent);
}

void AEnvKnob::Refresh()
{
	if (mExtent == mDefaultExtent)
		return;

	((UBoxComponent*)RootComponent)->InitBoxExtent(mExtent);

	FVector meshExtent = FVector(mExtent.X, mExtent.Y, mExtent.Z + mVisualHeight);
	mMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -meshExtent.Z));
	mMeshComponent->SetWorldScale3D(meshExtent / 50);

	FVector triggerExtent = FVector(mExtent.X, mExtent.Y, mExtent.Z + mCollisionHeight);
	mBoxComponent->SetBoxExtent(triggerExtent);

	if (canInteractWithBone)
		canBeToggledOnOff = false;
}

void AEnvKnob::OnConstruction(const FTransform & Transform)
{
	Deactivate();
}

void AEnvKnob::Activate()
{
	if (!isCameraFollowingEnabled || !isFirstHit)
	{
		//Activate the environment object
		if (mActionObject)
			Cast<IEnvKnobInterface>(mActionObject)->KnobActivated();

		//Activate the cable
		if (mCable)
		{
			TArray<UStaticMeshComponent*> Components;
			mCable->GetComponents<UStaticMeshComponent>(Components);
			for (int32 i = 0; i < Components.Num(); i++)
			{
				Components[i]->SetMaterial(0, mActivatedMaterial);
			}
		}
	}

	//Activate following camera
	if (isCameraFollowingEnabled && isFirstHit)
	{
		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (levelManager != nullptr)
		{
			auto actor = levelManager->GetCurrentCharacter();
			m_followingCamera->SetCamera(actor, mActionObject);
			m_followingCamera->StartFollowing();
			//actor->SetEnableInput(false);
		}		
	}

	auto meshExtent = mExtent;
	//mMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -meshExtent.Z));
	mMeshComponent->SetWorldScale3D(meshExtent / 50);
}

void AEnvKnob::Deactivate()
{

	//Deactivate the environment object
	if (mActionObject)
		Cast<IEnvKnobInterface>(mActionObject)->KnobDeactivated();
	
	//Deactivate the cable
	if (mCable)
	{
		TArray<UStaticMeshComponent*> Components;
		mCable->GetComponents<UStaticMeshComponent>(Components);
		for (int32 i = 0; i < Components.Num(); i++)
		{
			Components[i]->SetMaterial(0, mDeactivatedMaterial);
		}
	}


	auto meshExtent = mExtent;
	meshExtent.Z += mVisualHeight;
	//mMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -meshExtent.Z));
	mMeshComponent->SetWorldScale3D(meshExtent / 50);
}

void AEnvKnob::ActivateEnvObject()
{
	if (isCameraFollowingEnabled && isFirstHit)
	{
		//Activate the environment object
		if (mActionObject)
			Cast<IEnvKnobInterface>(mActionObject)->KnobActivated();

		//Activate the cable
		if (mCable)
		{
			TArray<UStaticMeshComponent*> Components;
			mCable->GetComponents<UStaticMeshComponent>(Components);
			for (int32 i = 0; i < Components.Num(); i++)
			{
				Components[i]->SetMaterial(0, mActivatedMaterial);
			}
		}

		isFirstHit = false;
	}
}

void AEnvKnob::BeginPlay()
{
	Super::BeginPlay();
	m_followingCamera->m_OnCameraStartStay.AddDynamic(this, &AEnvKnob::ActivateEnvObject);
}


void AEnvKnob::OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{	
	if (Cast<ASplitGameCharacter>(OtherActor) && (OverlappedComponent->GetFName() == ACharacter::CapsuleComponentName) || (OtherComp->GetFName() == ACharacter::CapsuleComponentName))
	{
		mCharacters.Add(Cast<ASplitGameCharacter>(OtherActor));
	}
	else if (Cast<AEnvObject>(OtherActor) && this != OtherActor && OtherComp->GetFName() == "RootComponent")
	{
		mEnvironment.Add(Cast<AEnvObject>(OtherActor));
	}
	if (!isCurrentlyToggledOn && (mCharacters.Num() == 1 || (canInteractWithBone && Cast<ABoomerang>(OtherActor)) || (mEnvironment.Num() == 1 && canInteractWithEnvironment)))
	{
		isCurrentlyToggledOn = true;

		if (mActionObject && mActionObject->GetClass()->ImplementsInterface(UEnvKnobInterface::StaticClass()))
			Activate(); 

		
	}
}

void AEnvKnob::OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ASplitGameCharacter>(OtherActor) && (OverlappedComponent->GetFName() == ACharacter::CapsuleComponentName) || (OtherComp->GetFName() == ACharacter::CapsuleComponentName))
	{
		mCharacters.Remove(Cast<ASplitGameCharacter>(OtherActor));
	}
	else if (Cast<AEnvObject>(OtherActor) && this != OtherActor)
	{
		mEnvironment.Remove(Cast<AEnvObject>(OtherActor));
	}
	if (mCharacters.Num() == 0 && (!canInteractWithEnvironment || mEnvironment.Num() == 0) && canBeToggledOnOff)
	{
		isCurrentlyToggledOn = false;

		if (mActionObject && mActionObject->GetClass()->ImplementsInterface(UEnvKnobInterface::StaticClass()))
			Deactivate();

	}
}