// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include <SplitGame/SplitGameCharacter.h>
#include "FallingBoxComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoxKillSkeletonDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPLITGAME_API UFallingBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

protected:	
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	class UFollowingCameraComponent* GetFollowingCamera();

	FVector m_ownerInitPosition;

	class USFXComponent* mSFXThud;
public:

	UFallingBoxComponent();

	UFUNCTION(BlueprintCallable)
	void DestorySelf();

	UFUNCTION()
	void Reset(ASplitGameCharacter* i_character);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isOwnerAttachedByRope = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool shouldOwnerRespawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isSkeletonBelow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isBoxActive;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UFollowingCameraComponent* m_followingCamera;

	UFUNCTION(BlueprintCallable)
	void ResetPosition();

	UFUNCTION(BlueprintCallable)
	void SetBoxActive(bool i_active);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "FallingBox", meta = (DisplayName = "On Skeleton Killed"))
	FBoxKillSkeletonDelegate m_onSkeletonKilled;
};
