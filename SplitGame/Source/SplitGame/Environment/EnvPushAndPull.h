#pragma once

#include "CoreMinimal.h"
#include "EnvObject.h"
#include "FallingBoxComponent.h"
#include "EnvPushAndPull.generated.h"

class UBoxComponent;

UCLASS()
class SPLITGAME_API AEnvPushAndPull : public AEnvObject
{
	GENERATED_BODY()
			
	UBoxComponent* mTriggerComponent;

	UStaticMeshComponent* mMeshComponent;	

	int bIsAwake = 1;

	FVector mDefaultExtent = FVector(136.f, 136.f, 136.f);
	
public:	
	AEnvPushAndPull();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* mBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* mStaticMesh;

	void OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector mExtent = mDefaultExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFallingBoxComponent* mFallingBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector mFallingBoxExtents = mDefaultExtent;

	virtual void Refresh();

	virtual void BeginPlay();

	virtual void Action(ASplitGameCharacter* Character);

	virtual void Tick(float DeltaTime) override;

	class USFXComponent* mSFXSliding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsAttachedByRope;
};
