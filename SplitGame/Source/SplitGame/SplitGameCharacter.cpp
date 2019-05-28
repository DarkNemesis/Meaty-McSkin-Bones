// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SplitGameCharacter.h"
#include "Component/SplitCameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime/Engine/Classes/Engine/EngineTypes.h>
#include <Runtime/Engine/Public/TimerManager.h>
#include "Level/LevelManager.h"
#include "SkeletonCharacter.h"
#include "SkinCharacter.h"
#include "MuscleCharacter.h"
#include <SplitGame/Environment/FallingBoxComponent.h>
#include <Misc/OutputDeviceNull.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include "Component/SFXComponent.h"

#include <string>

ASplitGameCharacter::ASplitGameCharacter()
{
	Initialize();
}

ASplitGameCharacter::ASplitGameCharacter(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	Initialize();
}

ASplitGameCharacter::~ASplitGameCharacter()
{}

void ASplitGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInteractionInputState)
		InteractionInputHeld();

	if (bActionInputState)
		ActionInputHeld();

	auto actorLocation = GetActorLocation();
	actorLocation.X = 0;
	SetActorLocation(actorLocation);

	if (mWidgetComponent && mWidgetComponent->IsValidLowLevel())
	{
		mWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(mWidgetComponent->GetComponentLocation(), SideViewCameraComponent->GetComponentLocation()));
	}

	if (bJustSwitched)
		bJustSwitched = false;
}

void ASplitGameCharacter::Initialize()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetMesh()->SetRelativeRotation(FRotator(0, 90, 0));

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 2350.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 0.f);	
	
	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<USplitCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera
	SideViewCameraComponent->SetFieldOfView(70.f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = mDefaultRotationRate; // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 0.6666f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;
	
	mTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	mTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mTriggerComponent->SetupAttachment(RootComponent);
	float radius; float height;
	GetCapsuleComponent()->GetScaledCapsuleSize(radius, height);
	auto triggerExtent = FVector(radius + 55, radius + 55, height);
	mTriggerComponent->InitBoxExtent(triggerExtent);
	mTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ASplitGameCharacter::OnCharInteractiveBegin);
	mTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ASplitGameCharacter::OnCharInteractiveEnd);

	mOnCharacterDead.AddDynamic(this, &ASplitGameCharacter::OnCharacterDead);
	mOnCharacterReset.AddDynamic(this, &ASplitGameCharacter::OnCharacterReset);

	m_initPosition = GetActorLocation();

	mRespawnSFXComponent = CreateDefaultSubobject<USFXComponent>(TEXT("RespawnSFXComponent"));
	mRespawnSFXComponent->SetupAttachment(RootComponent);
	mRespawnSFXComponent->AddAudio("SoundWave'/Game/SFX/Respawn/MMSB_EVENT_RESPAWN_006.MMSB_EVENT_RESPAWN_006'", USFXComponent::SoundWave);
	mRespawnSFXComponent->AddAudio("SoundWave'/Game/SFX/Respawn/MMSB_EVENT_RESPAWN_013.MMSB_EVENT_RESPAWN_013'", USFXComponent::SoundWave);

}

void ASplitGameCharacter::OnCharInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) 
{
	//if (bIsPiloted && Cast<ASplitGameCharacter>(OtherActor))
	//{
	//	mInteractingChar = Cast<ASplitGameCharacter>(OtherActor);
	//}	

	if (Cast<ASplitGameCharacter>(OtherActor) && !OtherComp->IsA(UCapsuleComponent::StaticClass()))
	{
		mInteractingChar = Cast<ASplitGameCharacter>(OtherActor);
	}

}
void ASplitGameCharacter::OnCharInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if (bIsPiloted && mInteractingChar == Cast<ASplitGameCharacter>(OtherActor))
	//{
	//	mInteractingChar = nullptr;
	//}

	if (mInteractingChar == Cast<ASplitGameCharacter>(OtherActor) && !OtherComp->IsA(UCapsuleComponent::StaticClass()))
	{
		mInteractingChar = nullptr;
	}

}

void ASplitGameCharacter::OnEnvInteractiveBegin(AEnvObject* EnvObject)
{}
void ASplitGameCharacter::OnEnvInteractiveEnd(AEnvObject* EnvObject)
{}

bool ASplitGameCharacter::IsCharInteracting()
{
	return false;
}

void ASplitGameCharacter::SetPointer()
{
	mWidgetComponent = NewObject<UWidgetComponent>(this, UWidgetComponent::StaticClass(), "HUD");
	mWidget = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/UI/BP_Pointer.BP_Pointer_C"));
	mWidgetComponent->SetWidgetClass(mWidget);
	mWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mWidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	mWidgetComponent->SetupAttachment(RootComponent);
	mWidgetComponent->SetRelativeRotation(FRotator(0, 0, 0));
	mWidgetComponent->SetRelativeLocation(FVector(0, 0, 130));
	mWidgetComponent->SetDrawSize(FVector2D(70, 70));
	mWidgetComponent->RegisterComponent();
}

void ASplitGameCharacter::RemovePointer()
{
	if (mWidgetComponent->IsValidLowLevel())
	{
		mWidgetComponent->DestroyComponent();
	}
}

void ASplitGameCharacter::ForceHorizontalRotate()
{
	auto rot = GetActorRotation();

	if (rot.Yaw > 0 && rot.Yaw <= 180.0f)
	{
		auto newRot = FRotator(rot.Pitch, 90.0f, rot.Roll);
		SetActorRotation(newRot);
	}
	else if (rot.Yaw <= 0 && rot.Yaw >= -180.0f)
	{
		auto newRot = FRotator(rot.Pitch, -90.0f, rot.Roll);
		SetActorRotation(newRot);
	}
}

void ASplitGameCharacter::OnCharacterDead(ASplitGameCharacter * character)
{
	UWorld* world = GetWorld();
	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if (levelManager)
	{
		if (levelManager->m_SaveSystem)
		{
			FOutputDeviceNull ar;
			if (Cast<AMuscleCharacter>(this))
			{
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("StopAutoSave 0"), ar, NULL, true);
			}
			else if (Cast<ASkeletonCharacter>(this))
			{
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("StopAutoSave 1"), ar, NULL, true);
			}
			else if (Cast<ASkinCharacter>(this))
			{
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("StopAutoSave 2"), ar, NULL, true);
			}
		}
	}

	
	SetEnableInput(false);
	auto currentLevel = GetWorld()->GetMapName();
	currentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	FName levelName = *currentLevel;
	FTimerHandle handle;
	FTimerDelegate deadDelegate;
	//deadDelegate.BindStatic<const UObject*, FName, bool, FString>(&UGameplayStatics::OpenLevel, GetWorld(), levelName, true, "");
	deadDelegate.BindUFunction(this, "LoadGame");
	GetWorld()->GetTimerManager().SetTimer(handle, deadDelegate, DeadDelay, false, -1);

	auto skin = Cast<ASkinCharacter>(character);
	if (skin != nullptr)
	{
		skin->PlayDeadAudio();
	}
	
}

void ASplitGameCharacter::OnCharacterReset(ASplitGameCharacter * character)
{
	if (InputComponent)
	{
		SetEnableInput(true);
	}

	mRespawnSFXComponent->Play();
}

void ASplitGameCharacter::SetEnableInput(bool key)
{		
	if (InputComponent != nullptr)
	{
		if (key)
		{
			SetupPlayerInputComponent(InputComponent);
		}
		else
		{
			auto num = InputComponent->GetNumActionBindings();
			/*for (int32 i = 0; i < num; ++i)
			{
				InputComponent->RemoveActionBinding(i);
			}*/
			InputComponent->ClearActionBindings();
			InputComponent->AxisBindings.Empty();
			//num = InputComponent->AxisBindings.Num();
			//for (int32 i = 0; i < num; ++i)
			//{
			//	
			//}
		}
	}
}

void ASplitGameCharacter::LoadGame()
{
	UWorld* world = GetWorld();
	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if (levelManager)
	{
		if (levelManager->m_SaveSystem)
		{
			FOutputDeviceNull ar;

			if (Cast<AMuscleCharacter>(this))
			{
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("LoadLevelData 0"), ar, NULL, true);
			}
			else if (Cast<ASkeletonCharacter>(this))
			{
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("LoadLevelData 1"), ar, NULL, true);
			}
			else if (Cast<ASkinCharacter>(this))
			{
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("LoadLevelData 2"), ar, NULL, true);
			}
		}		
	}
}

FVector ASplitGameCharacter::GetInitPosition()
{
	return m_initPosition;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASplitGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ASplitGameCharacter::ActionInputPressed);
	PlayerInputComponent->BindAction("Action", IE_Released, this, &ASplitGameCharacter::ActionInputReleased);

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ASplitGameCharacter::InteractionInputPressed);
	PlayerInputComponent->BindAction("Interaction", IE_Released, this, &ASplitGameCharacter::InteractionInputReleased);

	PlayerInputComponent->BindAction("InteractionCancel", IE_Released, this, &ASplitGameCharacter::InteractionInputCancelled);

	PlayerInputComponent->BindAxis("Walk", this, &ASplitGameCharacter::MoveLeftRight);

	PlayerInputComponent->BindAxis("PeekHorizontal", SideViewCameraComponent, &USplitCameraComponent::PeekLeftRight);
	PlayerInputComponent->BindAxis("PeekVertical", SideViewCameraComponent, &USplitCameraComponent::PeekUpDown);

	
}

void ASplitGameCharacter::MoveLeftRight(float Value)
{
	// add movement in that direction
	if (Value < 0.f)
		Value = -1.0f;
	else if (Value > 0.f)
		Value = 1.0f;

	AddMovementInput(FVector(0.f, 1.f, 0.f), Value);
}

void ASplitGameCharacter::ActionInputPressed()
{
	bActionInputState = true;
	OnActionInputPressed();
}

void ASplitGameCharacter::ActionInputReleased()
{
	bActionInputState = false;
	OnActionInputReleased();
}

void ASplitGameCharacter::ActionInputHeld()
{
	OnActionInputHeld();
}

void ASplitGameCharacter::InteractionInputPressed()
{
	bInteractionInputState = true;
	OnInteractionInputPressed();
}
void ASplitGameCharacter::InteractionInputReleased()
{
	bInteractionInputState = false;
	OnInteractionInputReleased();
}

void ASplitGameCharacter::InteractionInputHeld()
{
	OnInteractionInputHeld();
}

void ASplitGameCharacter::InteractionInputCancelled()
{
	OnInteractionInputCancelled();
}

void ASplitGameCharacter::OnActionInputPressed() {}
void ASplitGameCharacter::OnActionInputReleased() {}
void ASplitGameCharacter::OnActionInputHeld() {}

void ASplitGameCharacter::OnInteractionInputPressed() {}
void ASplitGameCharacter::OnInteractionInputReleased() {}
void ASplitGameCharacter::OnInteractionInputHeld() {}
void ASplitGameCharacter::OnInteractionInputCancelled() {}

void ASplitGameCharacter::OnSwitchFrom() {}
void ASplitGameCharacter::OnSwitchTo() {}

void ASplitGameCharacter::Restart()
{
	bJustSwitched = true;

	Super::Restart();
}

