// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvRope.generated.h"

UCLASS()
class SPLITGAME_API AEnvRope : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvRope();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttachObject", meta = (AllowPrivateAccess = "true"))
	AActor* m_platform;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//TODO
	UFUNCTION(BlueprintCallable)
	void EnablePlatformGravity(bool i_enable);


};
