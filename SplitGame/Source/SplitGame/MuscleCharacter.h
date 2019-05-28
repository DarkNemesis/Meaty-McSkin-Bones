#pragma once

#include "SplitGameCharacter.h"
#include "Component/SFXComponent.h"
#include "MuscleCharacter.generated.h"

class ABoomerang;

UENUM(BlueprintType)
enum class EMuscleSFX : uint8
{
	WalkOnHardSurface,
	WalkOnCheese,
	BreakWall,
	Grunt,
	Celebration,
	PushBox,
	Land,
	RipBone,
	Count
};

UCLASS()
class SPLITGAME_API AMuscleCharacter : public ASplitGameCharacter
{
	GENERATED_BODY()

	

public:
	AMuscleCharacter();
	virtual ~AMuscleCharacter() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bIsHoldingBone = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bIsCarrySkeleton = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SFX")
		TMap<EMuscleSFX, FSfxSources> mSoundSources;

protected:
	virtual void BeginPlay() override;
	virtual void Jump() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

public:
	void OnCharInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;
	void OnCharInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual void Tick(float DeltaTime) override;
	
	void OnActionInputPressed() override;
	void OnActionInputReleased() override;
	void OnActionInputHeld() override;

	UFUNCTION(BlueprintCallable)
	int GetPushingDirection();

	UFUNCTION(BlueprintCallable)
		void PlaySFX(EMuscleSFX type);

	UFUNCTION(BlueprintCallable)
		void StopSFX(EMuscleSFX type);

	virtual bool IsCharInteracting();
		
	void OnEnvInteractiveBegin(AEnvObject* EnvObject);
	
	void OnEnvInteractiveEnd(AEnvObject* EnvObject);

	void OnInteractionInputPressed();

	void OnInteractionInputReleased();

private:
	TMap<EMuscleSFX, class USFXComponent*> mSFXComponents;

};
