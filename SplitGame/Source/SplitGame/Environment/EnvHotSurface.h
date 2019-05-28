// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvObject.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "EnvKnobInterface.h"
#include "EnvHotSurface.generated.h"

UCLASS()
class SPLITGAME_API AEnvHotSurface : public AEnvObject, public IEnvKnobInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvHotSurface();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* m_mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector m_Extent = m_DefaultExtent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector m_TriggerExtent = m_DefaultTriggerExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface Material")
	UMaterial* m_surfaceOnMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Surface Material")
	UMaterial* m_surfaceOffMaterial = nullptr;

	UMaterialInstanceDynamic* DynamicMaterialInst;

	void TurnOnSurface(bool surfaceOn);

	UFUNCTION(Category = "Knob Activation")
	virtual bool KnobActivated() override;

	UFUNCTION(Category = "Knob Deactivation")
	virtual bool KnobDeactivated() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extents")
	FVector m_DefaultExtent = FVector(100.0f, 100.0f, 20.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extents")
	FVector m_DefaultTriggerExtent = FVector(100.0f, 100.f, 40.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extents")
	FVector m_DefaultEnterTriggerExtent = FVector(100.0f, 50.0f, 100.0f);

private:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* m_collider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* m_trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* m_leftEnterTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* m_rightEnterTrigger;
	
	void OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	void OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void SetRespawnLocation(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	bool m_bIsSurfaceOn;

	class USFXComponent* mSFXSliding;
};
