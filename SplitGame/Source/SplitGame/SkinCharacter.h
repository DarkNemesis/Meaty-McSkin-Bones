#pragma once
#include "SplitGameCharacter.h"
#include "Component/SkinBalloonComponent.h"
#include "SkinCharacter.generated.h"

class USkinCatapultComponent;
class USkinBalloonComponent;

UENUM(BlueprintType)
enum class eSkinStates : uint8
{
	Skin,
	Balloon
};

UCLASS()
class SPLITGAME_API ASkinCharacter : public ASplitGameCharacter
{
	GENERATED_BODY()

public:
	//Public Blueprint exposed variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
		eSkinStates m_CurrentSkinState = eSkinStates::Skin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties")
		float m_FloatTime = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Properties")
		float m_FloatGravity = 0.25f / 3.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Properties")
		float m_StopForce = 1.0f;

public:
	//Public variables for Ballon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		USkinBalloonComponent* m_Balloon;


public:
	//Blueprint exposed funtions
	UFUNCTION(BlueprintCallable)
		void SetGravityScale(float i_GravityScale);

public:
	ASkinCharacter();

	//Override from Base
	void OnActionInputPressed() override;
	void OnActionInputReleased() override;

	void OnInteractionInputPressed() override;
	void OnInteractionInputReleased() override;

	void OnInteractionInputCancelled() override;

	void OnSwitchFrom() override;

	void OnCharInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;
	void OnCharInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Tick(float DeltaTime) override;
	void BeginPlay() override;
	bool GetInputState();

	virtual bool IsCharInteracting();

	void SetIsOnAirVent(bool i_Bool);

	bool GetIsCharacterFloating();

	void PlayDeadAudio();

	void PlayCelebrationAudio();

protected:
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void OnCharacterReset(ASplitGameCharacter* character) override;



private:
	float m_CurrentFloatTime;
	FTransform m_CharacterInitialTransform;
	bool b_IsCharacterFloating;
	bool b_IsOnAirVent;

	class USFXComponent* m_GlideAudio;
	class USFXComponent* m_InAirVentAudio;
	USkinCatapultComponent* mCatapult = nullptr;
	class USFXComponent* mSlideSFXComponent;
	class USFXComponent* mDeadAudio;
	class USFXComponent* mBurnAudio;
	class USFXComponent* mCelebrationAudio;
};