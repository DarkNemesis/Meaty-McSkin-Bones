// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SplitGameCharacter.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "SkinCharacter.h"
#include "Component/SFXComponent.h"
#include "SkeletonCharacter.generated.h"

UENUM(BlueprintType)
enum class ESkeletonCustomMoveMode : uint8
{
	Climb,
	OnMuscle,
	Dead
};

UENUM(BlueprintType)
enum class ESkeletonCustomAnimation : uint8
{
	None,
	ClimbUpEnd,
	ClimbDownStart,
	ClimbOnMuscle,
	Dead
};

UENUM(BlueprintType)
enum class ESkeletonSFX : uint8
{
	ArmDetach,
	ClimbOnShoulder,
	ClimbLoop,
	IdleWithRibcage,
	HardSurface,
	MeltedCheese,
	Cheese,
	Celebration,
	Confusion,
	Scared,
	BonesBreak,
	Death,
	Count
};

UENUM(BlueprintType)
enum class ESkeletonBoomerangAnimation: uint8
{
	None,
	Throw,
	Catch
};

UCLASS()
class SPLITGAME_API ASkeletonCharacter : public ASplitGameCharacter
{
	GENERATED_BODY()
	
public:
	ASkeletonCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual ~ASkeletonCharacter() override;

	bool bCanClimbDown = false;
	bool bIsOnMuscle = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool b_IsCharacterFloating = false;

	UFUNCTION()
		void StartClimb();
	UFUNCTION()
		void StopClimb();
	UFUNCTION()
		bool ClimbDown();
	UFUNCTION()
		void ClimbOffMuscle();

	UFUNCTION(BlueprintPure)
		ESkeletonCustomAnimation GetAnimationType() { return animationType; }
	UFUNCTION(BlueprintCallable)
		void SetAnimationType(ESkeletonCustomAnimation animation) { animationType = animation; }

	UFUNCTION(BlueprintPure)
		ESkeletonBoomerangAnimation GetBoomerangAnimationType() { return boomerangAnimeType; }
	UFUNCTION(BlueprintCallable)
		void SetBoomerangAnimationType(ESkeletonBoomerangAnimation animation) { boomerangAnimeType = animation; }

	UFUNCTION(BlueprintCallable)
		void SetGravityScale(float i_GravityScale);

	UFUNCTION(BlueprintPure)
		bool IsCarringSkin() { return isCarryingSkin; }

	UFUNCTION(BlueprintCallable)
		void PlaySFX(ESkeletonSFX type);

	UFUNCTION(BlueprintCallable)
		void StopSFX(ESkeletonSFX type);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BoomeRang")
	bool isBoomerangReady;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SFX")
	 TMap<ESkeletonSFX, FSfxSources> mSoundSources;

	virtual bool IsCharInteracting();

	void OnCharInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;
	void OnCharInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void OnEnvInteractiveBegin(AEnvObject* EnvObject) override;
	virtual void OnEnvInteractiveEnd(AEnvObject* EnvObject) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climb")
		float ClimbSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties")
		float m_FloatGravity = 0.25f;

	virtual void OnActionInputPressed() override;
	virtual void OnActionInputReleased() override;
	virtual void OnInteractionInputPressed() override;
	virtual void OnInteractionInputReleased() override;
	virtual void OnInteractionInputCancelled() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	virtual void Jump() override {}

	virtual void Restart() override;

	virtual void OnCharacterReset(ASplitGameCharacter* character) override;
	virtual void OnCharacterDead(ASplitGameCharacter* character) override;


	void Climb(float value);

	
	bool isCarryingSkin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
		ASkinCharacter* m_skinCharacter;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level", meta = (AllowPrivateAccess = "true"))
		FName LevelName;
	UPROPERTY(EditDefaultsonly, Category = "Projectile")
		TSubclassOf<class ABoomerang> projectileClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		class USkeletonDieComponent* DieOfFallingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* DeathMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* DeathHitMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		ESkeletonCustomAnimation animationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		ESkeletonBoomerangAnimation boomerangAnimeType;

	void ShootBoomerang();
	void StartFloating();
	void EndFloating();

	void CarrySkin();
	void DropSkin();

	class AEnvClimbable* mpClimbableObject = nullptr;
	TMap<ESkeletonSFX, class USFXComponent*> mSFXComponents;

};
