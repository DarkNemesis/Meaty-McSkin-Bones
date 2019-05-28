#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SplitGameCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDeadDelegate, ASplitGameCharacter*, DeadCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterResetDelegate, ASplitGameCharacter*, ResetCharacter);

class AEnvObject;
class UPointerWidget;
class UWidgetComponent;

UCLASS(config=Game)
class ASplitGameCharacter : public ACharacter
{
	GENERATED_BODY()

	void Initialize();
	
	void ActionInputPressed();
	void ActionInputReleased();
	void ActionInputHeld();

	void InteractionInputPressed();
	void InteractionInputReleased();
	void InteractionInputHeld();

	void InteractionInputCancelled();

	void MoveLeftRight(float Val);
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//True if this character is being controlled by the player
	bool bIsPiloted;

	//True if the Action Input is being held
	bool bActionInputState = false;

	//True if the Interaction Input is being held
	bool bInteractionInputState = false;

	//True if the character is dead
	bool bIsDead;
	
	FRotator mDefaultRotationRate = FRotator(0.0f, 720.0f, 0.0f);

	//An Environment Object that is in range and can be interacted with
	AEnvObject* mInteractingEnv = nullptr;

	//A Character that is in range and can be interacted with
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ASplitGameCharacter* mInteractingChar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USplitCameraComponent* SideViewCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplitCharacter")
		float DeadDelay = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trigger, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* mTriggerComponent;

	UFUNCTION()
		virtual void OnCharacterDead(ASplitGameCharacter* character);

	UFUNCTION()
		virtual void OnCharacterReset(ASplitGameCharacter* character);

public:

	void SetEnableInput(bool key);
	ASplitGameCharacter();
	ASplitGameCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual ~ASplitGameCharacter();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class USplitCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void OnActionInputPressed();
	virtual void OnActionInputReleased();
	virtual void OnActionInputHeld();

	virtual void OnInteractionInputPressed();
	virtual void OnInteractionInputReleased();
	virtual void OnInteractionInputHeld();

	virtual void OnInteractionInputCancelled();

	virtual void OnSwitchFrom();
	virtual void OnSwitchTo();

	virtual void Restart() override;

	//This function is called by the Unreal Collision system when two characters can interact with each other.
	//This function should be handled only by the current piloted character.
	UFUNCTION()		
	virtual void OnCharInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
		
	//This function is called by the Unreal Collision system when two characters can no longer interact with each other.
	//This function should be handled only by the current piloted character.
	UFUNCTION()
	virtual void OnCharInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//This function is called by an Environment object when it can be interacted by a character
	UFUNCTION()
	virtual void OnEnvInteractiveBegin(AEnvObject* EnvObject);

	//This function is called by an Environment object when it can no longer be interacted by a character
	UFUNCTION()
	virtual void OnEnvInteractiveEnd(AEnvObject* EnvObject);
		
	UFUNCTION()
	virtual bool IsCharInteracting();

	UFUNCTION(BlueprintPure, Category = "SplitCharacter")
	bool IsDead() { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "SplitCharacter")
	void SetDead(bool isDead) 
	{ 
		bIsDead = isDead; 
		if(bIsDead) 
			mOnCharacterDead.Broadcast(this);
		else 
			mOnCharacterReset.Broadcast(this);
	}

	UFUNCTION(BlueprintPure, Category = "SplitCharacter")
		bool IsPiloted() { return bIsPiloted; }

	UFUNCTION(BlueprintCallable, Category = "SplitCharacter")
		void SetPiloting(bool pilot) { bIsPiloted = pilot; }

	UFUNCTION(BlueprintCallable, Category = "UI")
		void SetPointer();

	UFUNCTION(BlueprintCallable, Category = "SplitCharacter")
		void RemovePointer();

	void ForceHorizontalRotate();

	UFUNCTION()
		void LoadGame();

	UPROPERTY(BlueprintAssignable, Category = "SplitCharacter", meta = (DisplayName = "On Character Dead"))
		FCharacterDeadDelegate mOnCharacterDead;

	UPROPERTY(BlueprintAssignable, Category = "SplitCharacter", meta = (DisplayName = "On Character Reset"))
		FCharacterResetDelegate mOnCharacterReset;

	FVector GetInitPosition();

	void SetJustSwitched(bool switched) { bJustSwitched = switched; }
	bool GetJustSwitched() { return bJustSwitched; }

private:

		UPROPERTY()
			UWidgetComponent* mWidgetComponent;

		UPROPERTY()
			TSubclassOf<class UUserWidget> mWidget;

		class USFXComponent* mRespawnSFXComponent;

		FVector m_initPosition;
		bool bJustSwitched = false;
};
