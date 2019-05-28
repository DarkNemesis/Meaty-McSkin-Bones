// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvRope.h"
#include "EnvPushAndPull.h"
#include "Components/PrimitiveComponent.h"


// Sets default values
AEnvRope::AEnvRope()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

}

// Called when the game starts or when spawned
void AEnvRope::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void AEnvRope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnvRope::EnablePlatformGravity(bool i_enable)
{
	auto* collider = m_platform->GetComponentByClass(UPrimitiveComponent::StaticClass());
	if (collider != nullptr)
	{
		Cast<UPrimitiveComponent>(collider)->SetEnableGravity(i_enable);
	}
}


