// Fill out your copyright notice in the Description page of Project Settings.

#include "SkeletonCharacter.h"
#include "Component/SkeletonDieComponent.h"
#include "Component/SkeletonCharacterMovement.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "SplitGame/Boomerang.h"
#include "MuscleCharacter.h"
#include "SkinCharacter.h"
#include "Engine/EngineTypes.h"
#include "Environment/EnvClimbable.h"
#include "Level/LevelManager.h"
#include "Misc/OutputDeviceNull.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Environment/FallingBoxComponent.h"
#include "Component/SFXComponent.h"
#include "UI/InteractionButtonWidget.h"


ASkeletonCharacter::~ASkeletonCharacter()
{
}

void ASkeletonCharacter::OnActionInputPressed()
{
	
}

void ASkeletonCharacter::OnActionInputReleased()
{
	auto onGround = GetCharacterMovement()->MovementMode == MOVE_Walking;
	if (onGround && isBoomerangReady && !isCarryingSkin)
	{
		//b_IsCharacterFloating = false;
		isBoomerangReady = false;
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &ASkeletonCharacter::ShootBoomerang, 0.3f, false);
		boomerangAnimeType = ESkeletonBoomerangAnimation::Throw;
	}


	if (isCarryingSkin && !onGround)
	{
		if (!b_IsCharacterFloating)
		{
			//StartFloating();
		}
		else
		{
			//EndFloating();
		}
	}

}

void ASkeletonCharacter::OnInteractionInputPressed()
{
}

void ASkeletonCharacter::OnInteractionInputReleased()
{
	if (mInteractingChar != nullptr)
	{
		AMuscleCharacter* muscleCharacter = Cast<AMuscleCharacter>(mInteractingChar);
		if (muscleCharacter && !isCarryingSkin && GetCharacterMovement()->MovementMode == MOVE_Walking)
		{
			Cast<USkeletonCharacterMovement>(GetCharacterMovement())->ClimbOnMuscle(muscleCharacter);
			ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
			if (levelManager)
			{
				levelManager->ShowInteractionWidgetForType(EInteractionTypes::Cancel);
			}
		}

		if (isBoomerangReady && boomerangAnimeType != ESkeletonBoomerangAnimation::Catch && boomerangAnimeType != ESkeletonBoomerangAnimation::Throw)
		{
			CarrySkin();
			ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
			if (levelManager)
			{
				levelManager->ShowInteractionWidgetForType(EInteractionTypes::Cancel);
			}
		}
	}	


}

void ASkeletonCharacter::OnInteractionInputCancelled()
{
	StopClimb();
	ClimbOffMuscle();

	auto onGround = GetCharacterMovement()->MovementMode == MOVE_Walking;
	if(onGround)
		DropSkin();
}

void ASkeletonCharacter::PlaySFX(ESkeletonSFX type)
{
	if (mSFXComponents.Contains(type))
	{
		mSFXComponents[type]->Play();
	}
}

void ASkeletonCharacter::StopSFX(ESkeletonSFX type)
{
	if (mSFXComponents.Contains(type))
	{
		mSFXComponents[type]->Stop();
	}
}

bool ASkeletonCharacter::IsCharInteracting()
{
	return isCarryingSkin || (GetCharacterMovement()->MovementMode == MOVE_Custom && GetCharacterMovement()->CustomMovementMode == (uint8)ESkeletonCustomMoveMode::OnMuscle);
}

void ASkeletonCharacter::OnCharInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASplitGameCharacter::OnCharInteractiveBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (((Cast<ASkinCharacter>(OtherActor) != nullptr) || (Cast<AMuscleCharacter>(OtherActor) != nullptr)) && !IsCharInteracting())
	{
		if (GetWorld()->GetFirstPlayerController()->GetCharacter()->IsA(ASkinCharacter::StaticClass()))
		{
			return;
		}
		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (OtherActor->IsA(ASkinCharacter::StaticClass()) && GetWorld()->GetFirstPlayerController()->GetCharacter()->IsA(ASkeletonCharacter::StaticClass()))
		{
			if (levelManager)
			{
				levelManager->ShowInteractionWidgetForType(EInteractionTypes::ParachuteInteraction);
			}
			return;
		}
		if (OtherActor->IsA(AMuscleCharacter::StaticClass()) && GetWorld()->GetFirstPlayerController()->GetCharacter()->IsA(ASkeletonCharacter::StaticClass()))
		{
			if (levelManager)
			{
				levelManager->ShowInteractionWidgetForType(EInteractionTypes::ClimbMuscleInteraction);
			}
		}
	}
}

void ASkeletonCharacter::OnCharInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASplitGameCharacter::OnCharInteractiveEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (!OtherActor->IsA(ASplitGameCharacter::StaticClass()) || OtherActor == this)
		return;

	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if (levelManager)
	{
		levelManager->RemoveInteractionWidget();
	}
}

void ASkeletonCharacter::OnEnvInteractiveBegin(AEnvObject * EnvObject)
{
	if (AEnvClimbable* climbable = Cast<AEnvClimbable>(EnvObject))
	{
		mpClimbableObject = climbable;
	}
}

void ASkeletonCharacter::OnEnvInteractiveEnd(AEnvObject * EnvObject)
{
	if (AEnvClimbable* climbable = Cast<AEnvClimbable>(EnvObject))
	{
		if(climbable == mpClimbableObject)
			mpClimbableObject = nullptr;
	}
}

void ASkeletonCharacter::ShootBoomerang()
{
	ABoomerang::Shoot(this);
}

void ASkeletonCharacter::StartFloating()
{	
	auto vel = GetCharacterMovement()->Velocity;
	auto newVel = FVector(vel.X, vel.Y, 0);
	GetCharacterMovement()->Velocity = FVector(newVel);
	b_IsCharacterFloating = true;
	SetGravityScale(0.25f);
}

void ASkeletonCharacter::EndFloating()
{
	b_IsCharacterFloating = false;
	SetGravityScale(2.0f / 3.0f);
	DieOfFallingComponent->StartRecord();
}

void ASkeletonCharacter::CarrySkin()
{
	if (mInteractingChar != nullptr && Cast<ASkinCharacter>(mInteractingChar) != nullptr && !isCarryingSkin && GetCharacterMovement()->MovementMode == MOVE_Walking)
	{

		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (levelManager)
		{
			levelManager->RemoveInteractionWidget();
		}

		isCarryingSkin = true;
		m_skinCharacter = Cast<ASkinCharacter>(mInteractingChar);
	}
}

void ASkeletonCharacter::DropSkin()
{
	if (isCarryingSkin)
	{
		isCarryingSkin = false;
		FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, false);
		m_skinCharacter->DetachFromActor(rules);
		m_skinCharacter->SetActorRotation(FQuat::Identity);
		m_skinCharacter = nullptr;
	}
}

ASkeletonCharacter::ASkeletonCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<USkeletonCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
		DieOfFallingComponent = CreateDefaultSubobject<USkeletonDieComponent>(TEXT("Skeleton Die Component"));
		AddOwnedComponent(DieOfFallingComponent);	
		DeathMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Death Mesh"));
		AddOwnedComponent(DeathMesh);
		DeathHitMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Death Hit Mesh"));
		AddOwnedComponent(DeathHitMesh);

		for (int i = 0; i < (uint8)ESkeletonSFX::Count; i++)
		{
			FName name(TEXT("SFX Component"), i);
			auto sfxComponent = CreateDefaultSubobject<USFXComponent>(name);
			sfxComponent->SetupAttachment(RootComponent);
			mSFXComponents.Add((ESkeletonSFX)i, sfxComponent);
		}		
}


// Called when the game starts or when spawned
void ASkeletonCharacter::BeginPlay()
{
	Super::BeginPlay();

	DieOfFallingComponent = Cast<USkeletonDieComponent>(GetComponentByClass(USkeletonDieComponent::StaticClass()));
	ABoomerang::SetBoomerangOwner(this);
	isBoomerangReady = true;
	isCarryingSkin = false;	
	m_skinCharacter = nullptr;
	boomerangAnimeType = ESkeletonBoomerangAnimation::None;

	DeathMesh->SetVisibility(true);
	DeathHitMesh->SetVisibility(true);

	for (auto It = mSoundSources.CreateIterator(); It; ++It)
	{
		FSfxSources& value = It->Value;
		auto sfxComponent = mSFXComponents[It->Key];
		for (int i = 0; i < value.sources.Num(); i++)
		{
			sfxComponent->AddAudio(value.sources[i]);
		}
	}

}

// Called every frame
void ASkeletonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_skinCharacter != nullptr)
	{
		auto pos = GetMesh()->GetSocketLocation(m_skinCharacter->GetAttachParentSocketName());
		m_skinCharacter->SetActorLocation(pos);
	}
}

// Called to bind functionality to input
void ASkeletonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("ClimbUp", this, &ASkeletonCharacter::Climb);
	PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &ASkeletonCharacter::StartClimb);
}

void ASkeletonCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PrevMovementMode);
	if (GetCharacterMovement()->MovementMode == MOVE_Falling && !b_IsCharacterFloating)
	{
		if (m_skinCharacter != nullptr && isCarryingSkin)
			StartFloating();
		else
		{
			DieOfFallingComponent->StartRecord();
		}

	}

	if (PrevMovementMode == MOVE_Walking && !GetJustSwitched() &&
		!(GetCharacterMovement()->MovementMode == MOVE_Custom && GetCharacterMovement()->CustomMovementMode == (uint8)ESkeletonCustomMoveMode::Dead)
		&& GetController()->IsA(APlayerController::StaticClass()))
	{
		// save
		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (levelManager)
		{
			if (levelManager->m_SaveSystem)
			{
				FOutputDeviceNull ar;
				levelManager->m_SaveSystem->CallFunctionByNameWithArguments(TEXT("SaveLevelCharacters 1"), ar, NULL, true);
			}
		}
	}

	if(PrevMovementMode == MOVE_Falling)
	{
		EndFloating();

		if (GetCharacterMovement()->MovementMode == MOVE_Walking)
			DieOfFallingComponent->EndRecord();
		else
			DieOfFallingComponent->Reset();
	}

}

void ASkeletonCharacter::Restart()
{
	if (USkeletonCharacterMovement* movementComponent = Cast<USkeletonCharacterMovement>(GetMovementComponent()))
	{
		if (movementComponent->MovementMode == MOVE_Custom)
			return;
	}

	Super::Restart();

}

void ASkeletonCharacter::OnCharacterReset(ASplitGameCharacter * character)
{
	DeathMesh->SetHiddenInGame(true);
	DeathHitMesh->SetHiddenInGame(true);
	GetMesh()->SetHiddenInGame(false);

	SetAnimationType(ESkeletonCustomAnimation::None);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking, 0);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	Super::OnCharacterReset(character);
}

void ASkeletonCharacter::OnCharacterDead(ASplitGameCharacter * character)
{
	GetMesh()->SetHiddenInGame(true);
	TArray<UPrimitiveComponent*> components;
	GetOverlappingComponents(components);

	for (auto overlappingComponent : components)
	{
		if (overlappingComponent->IsA(UFallingBoxComponent::StaticClass()))
		{
			DeathHitMesh->SetHiddenInGame(false);
			DeathHitMesh->InitAnim(true);
			DeathHitMesh->Play(false);
			Super::OnCharacterDead(character);
			return;
		}
	}

	DeathMesh->SetHiddenInGame(false);
	DeathMesh->InitAnim(true);
	DeathMesh->Play(false);
	Super::OnCharacterDead(character);

	return;
}

void ASkeletonCharacter::Climb(float value)
{
	USkeletonCharacterMovement* movement = Cast<USkeletonCharacterMovement>(GetCharacterMovement());
	if (movement)
	{
		movement->SetUpInput(value);
	}
}

void ASkeletonCharacter::StartClimb()
{
	auto movement = GetCharacterMovement();
	if (!isBoomerangReady || GetAnimationType() != ESkeletonCustomAnimation::None || (movement->MovementMode == MOVE_Custom && movement->CustomMovementMode == (uint8)ESkeletonCustomMoveMode::Climb))
		return;

	if (!mpClimbableObject)
	{
		// try to climb down
		if (!ClimbDown() )
		{
			if (!(movement->MovementMode != MOVE_Custom || movement->CustomMovementMode != (uint8)(ESkeletonCustomMoveMode::Climb)))
			{
				ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
				if (levelManager)
				{
					levelManager->ShowActionWidget();
					return;
				}
			}
		}
		return;
	}

	DropSkin();
	// reset character rotation and location
	FVector direction = (mpClimbableObject->GetActorLocation() - GetActorLocation()).Y >= 0 ? FVector(0, 1, 0) : FVector(0, -1, 0);
	float radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float halfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FHitResult hitResult;
	FCollisionQueryParams params;
	FCollisionShape shape;
	float bias = 1;
	shape.SetBox(FVector(bias, radius, halfHeight));
	params.AddIgnoredActor(this);
	GetWorld()->SweepSingleByChannel(hitResult, GetActorLocation(), GetActorLocation() + direction * 100, GetActorQuat(), 
		ECC_Pawn, shape, params);

	FVector offset = FVector::ZeroVector;
	if (hitResult.bBlockingHit)
		offset = direction * (hitResult.Distance - radius);

	SetActorRotation(FRotator(0, -direction.Y * 90, 0));
	AddActorWorldOffset(offset);
	GetCharacterMovement()->SetMovementMode(MOVE_Custom, (uint8)ESkeletonCustomMoveMode::Climb);
}

void ASkeletonCharacter::StopClimb()
{
	auto movement = GetCharacterMovement();

	if (movement->MovementMode == EMovementMode::MOVE_Custom && movement->CustomMovementMode == StaticCast<uint8>(ESkeletonCustomMoveMode::Climb) &&
		animationType != ESkeletonCustomAnimation::ClimbUpEnd && animationType != ESkeletonCustomAnimation::ClimbDownStart)
	{
		if (movement->IsMovingOnGround())
		{
			movement->SetMovementMode(MOVE_Walking);
		}
		else
		{
			movement->SetMovementMode(MOVE_Falling);
		}

	}
}

bool ASkeletonCharacter::ClimbDown()
{
	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
		return false;

	UWorld* world = GetWorld();
	check(world);

	FVector direction = GetActorForwardVector().Y <= 0 ? FVector(0, 1, 0) : FVector(0, -1, 0);

	float radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float halfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float traceDistance = radius * 4;
	FVector topStart = GetActorLocation();
	FVector topEnd = topStart + direction * traceDistance;
	FVector bottomEnd = topStart - FVector::UpVector * halfHeight;
	FVector bottomStart = bottomEnd + direction * traceDistance;

	FHitResult topResult, bottomResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	if (!world->SweepSingleByChannel(topResult, topStart, topEnd, GetActorQuat(), ECC_Pawn,
		GetCapsuleComponent()->GetCollisionShape(0.9f), params) && world->SweepSingleByChannel(bottomResult, bottomStart, bottomEnd, GetActorQuat(), ECC_Pawn,
			GetCapsuleComponent()->GetCollisionShape(), params))
	{
		if (bottomResult.Distance != 0)
		{
			auto actor = bottomResult.Actor.Get();
			auto climbableWall = Cast<AEnvClimbable>(actor);
			if (!climbableWall)
			{
				TArray<AActor*> attachedActors;
				bottomResult.Actor->GetAttachedActors(attachedActors);
				if (attachedActors.Num() > 0)
					climbableWall = Cast<AEnvClimbable>(attachedActors[0]);
			}

			if (climbableWall)
			{
				if (auto movement = GetCharacterMovement())
				{
					movement->SetMovementMode(MOVE_Custom, (uint8)ESkeletonCustomMoveMode::Climb);
					SetAnimationType(ESkeletonCustomAnimation::ClimbDownStart);
				}

				//mpClimbableObject = climbableWall;
				return true;
			}
		}
	}

	// try another direction
	FVector bottomEnd2 = bottomEnd - direction * traceDistance;
	FVector topEnd2 = topStart - direction * traceDistance;
	if (!world->SweepSingleByChannel(topResult, topEnd2, topStart, GetActorQuat(), ECC_Pawn,
		GetCapsuleComponent()->GetCollisionShape(0.9f), params) && world->SweepSingleByChannel(bottomResult, bottomEnd2, bottomStart, GetActorQuat(), ECC_Pawn,
			GetCapsuleComponent()->GetCollisionShape(), params))
	{
		if (bottomResult.Distance == 0)
			return false;
		auto climbableWall = Cast<AEnvClimbable>(bottomResult.Actor);
		if (!climbableWall)
		{
			TArray<AActor*> attachedActors;
			bottomResult.Actor->GetAttachedActors(attachedActors);
			if (attachedActors.Num() > 0)
				climbableWall = Cast<AEnvClimbable>(attachedActors[0]);
		}

		if (climbableWall)
		{
			AddActorWorldRotation(FRotator(0, 180, 0));
			if (auto movement = GetCharacterMovement())
			{
				movement->SetMovementMode(MOVE_Custom, (uint8)ESkeletonCustomMoveMode::Climb);
				SetAnimationType(ESkeletonCustomAnimation::ClimbDownStart);
			}

			//mpClimbableObject = climbableWall;
			return true;
		}
	}

	return false;
}

void ASkeletonCharacter::ClimbOffMuscle()
{
	USkeletonCharacterMovement* movement = Cast<USkeletonCharacterMovement>(GetCharacterMovement());
	movement->ClimbOffMuscle();
}

void ASkeletonCharacter::SetGravityScale(float i_GravityScale)
{
	GetCharacterMovement()->GravityScale = i_GravityScale;
}



