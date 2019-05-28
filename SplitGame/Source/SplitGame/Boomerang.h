// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "SplitGameCharacter.h"
#include "SkeletonCharacter.h"
#include "Boomerang.generated.h"

UCLASS()
class SPLITGAME_API ABoomerang : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABoomerang();
	bool IsShotBySkeleton;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsFirstOverlapWithCharacter;
	bool IsFirstHit;
	
	float m_flyingForwardDuration;
	float m_FlyForwardTimer;
	float m_BounceTimer;

	FTransform m_InitTransform;
	FVector m_InitLocation;
	FRotator m_InitRotation;


	enum FlyState
	{
		Flyforward,
		BounceBack,//Only happened after collide with sth
		FlyBack
	};
	FlyState m_FlyingState;

	void SwitchFlyState(FlyState i_state);
	
	void OnFlyForwardUpdate(float DeltaTime);
	void OnBounceBackUpdate(float DeltaTime);
	void OnFlyBackUpdate(float DeltaTime);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* mMeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float BackForce;

	UPROPERTY(EditAnywhere, Category="Bullet Attributes")
	float InitFlyingSpeed;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float MaxRange;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float Radius;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float BounceDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComponent;

	//UPROPERTY(EditAnywhere, Category = Movement)
	//UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
	void OnBoomerangOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
	void OnBoomerangHit(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetInitLocation(FVector i_pos);
	void SetInitRotation(FRotator i_rot);
	void SetInitTransform(FTransform trans);
	void Initialize();

	static void Shoot(ASplitGameCharacter* character);
	void Catch();
	void DestoryBoomerang();

	UFUNCTION(BlueprintCallable)
	void PlayCutRopeSFX();

	static ASkeletonCharacter* m_owner;

	static void SetBoomerangOwner(ASkeletonCharacter* i_owner);

	class USFXComponent* mSFXShoot;
	class USFXComponent* mSFXFlying;
	class USFXComponent* mSFXHit;
	class USFXComponent* mSFXCutRope;
};
