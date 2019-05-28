#pragma once

#include "CoreMinimal.h"
#include "EnvObject.h"
#include "Containers/Set.h"
#include "EnvKnob.generated.h"

class UBoxComponent;
class UMaterial;

UCLASS()
class SPLITGAME_API AEnvKnob : public AEnvObject
{
	GENERATED_BODY()
	
	UBoxComponent* mBoxComponent;

	FVector mDefaultExtent = FVector(40.0f, 40.f, 10.f);
	
	TSet<ASplitGameCharacter*> mCharacters;

	TSet<AEnvObject*> mEnvironment;

	bool isCurrentlyToggledOn = false;

	bool isFirstHit = true;	

public:
	AEnvKnob();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UFollowingCameraComponent* m_followingCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* mMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AEnvObject* mActionObject = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool canBeToggledOnOff = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool canInteractWithBone = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool canInteractWithEnvironment = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isCameraFollowingEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* mCable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mCollisionHeight = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mVisualHeight = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* mActivatedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* mDeactivatedMaterial;
	
	void OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector mExtent = mDefaultExtent;

	virtual void Refresh();

	virtual void OnConstruction(const FTransform & Transform);

	void Activate();
	void Deactivate();

	UFUNCTION()
	void ActivateEnvObject();

protected:
	virtual void BeginPlay() override;

};
