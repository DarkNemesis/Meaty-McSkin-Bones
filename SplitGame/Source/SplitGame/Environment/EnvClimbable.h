// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Environment/EnvObject.h"
#include "EnvClimbable.generated.h"

class UBoxComponent;

UCLASS()
class SPLITGAME_API AEnvClimbable : public AEnvObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvClimbable();

protected:
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//virtual void Refresh() override;
	virtual void Action(ASplitGameCharacter* Character) override;

	virtual void OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* mBoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* WallCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Component, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	FVector mDefaultBoxExtend = FVector(50, 50, 50);
	FVector mDefaultWallExtend = FVector(80, 80, 40);

};
