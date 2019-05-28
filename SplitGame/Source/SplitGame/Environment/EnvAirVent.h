// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvObject.h"
#include "EnvKnobInterface.h"
#include "EnvAirVent.generated.h"

class UBoxComponent;
class UArrowComponent;
class UParticleSystemComponent;
class ASkinCharacter;
class ASkeletonCharacter;
UCLASS()
class SPLITGAME_API AEnvAirVent : public AEnvObject, public IEnvKnobInterface
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
		virtual bool KnobActivated() override;

	UFUNCTION(BlueprintCallable)
		virtual bool KnobDeactivated() override;
	
	static FName mAirColumnColliderFName;

public:	
	// Sets default values for this actor's properties
	AEnvAirVent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float m_Force = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float m_SkeletonForce = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		bool CanPull = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		bool b_CanTurnOnAndOff = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		bool b_IsTurnedOn = true;
		

	virtual void PostLoad() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent * Mesh;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	//	UBoxComponent* BoxCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UArrowComponent* ArrowComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent * ForegroundEffectMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent * BackgroundEffectMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void ChangeAirVentState(bool i_State);
	UFUNCTION(BlueprintCallable, Category = "Airvent Data")
		bool GetAirVentState() { return b_HasTurnedOn; }

private:
	UFUNCTION()
	void OnActorBeginOverlapLocal(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void OnActorEndOverlapLocal(AActor* OverlappedActor, AActor* OtherActor);

private:
	bool b_HasTurnedOn;
	bool b_HasOverLapped;
	
	ASkinCharacter* m_OverlappedSkin;
	ASkeletonCharacter* m_OverlappedSkeleton;

	class USFXComponent* m_AirVentStart;
	class USFXComponent* m_AirVentStop;
	class USFXComponent* m_AirVentAmbient;
};
