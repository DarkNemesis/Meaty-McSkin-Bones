// Fill out your copyright notice in the Description page of Project Settings.

#include "FollowingCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include <SplitGame/SplitGameCharacter.h>
#include <SplitGame/Environment/EnvObject.h>

#include <SplitGame/Component/SplitCameraComponent.h>

// Sets default values for this component's properties
UFollowingCameraComponent::UFollowingCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MoveOutTime = 1.0f;
	MoveBackTime = 0.01f;
	StayTime = 0.5f;
	m_state = Stop;

	m_character = nullptr;
	SideViewCameraComponent = nullptr;
	CameraBoom = nullptr;
}

void UFollowingCameraComponent::SetCamera(ASplitGameCharacter * i_currentCharacter, AActor* i_target)
{
	SideViewCameraComponent = i_currentCharacter->GetSideViewCameraComponent();
	CameraBoom = i_currentCharacter->GetCameraBoom();
	auto x = SideViewCameraComponent->GetComponentLocation().X;
	m_targetLocation = i_target->GetActorLocation();
	m_targetLocation.X = x;
	m_characterLocation = i_currentCharacter->GetActorLocation();
	m_characterLocation.X = x;
	m_character = i_currentCharacter;
	SideViewCameraComponent->SetEnableCamera(false);
}

void UFollowingCameraComponent::SetTargetLocation(FVector i_location)
{
	m_targetLocation = i_location;
}

// Called when the game starts
void UFollowingCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UFollowingCameraComponent::OnMoveOutUpdate(float deltaTime)
{
	auto cameraLocation = SideViewCameraComponent->GetComponentLocation();
	auto difference = m_targetLocation - cameraLocation;
	auto stride = difference / m_timer * deltaTime;
	SideViewCameraComponent->SetWorldLocation(cameraLocation + stride);

	if (m_timer > 0)
	{
		m_timer -= deltaTime;
		if (m_timer <= 0)
			SwitchState(Stay);
	}
}

void UFollowingCameraComponent::OnStayUpdate(float deltaTime)
{
	if (m_timer > 0)
	{
		m_timer -= deltaTime;
		if (m_timer <= 0)
			SwitchState(MoveBack);
	}
}

void UFollowingCameraComponent::OnMoveBackUpdate(float deltaTime)
{
	auto cameraLocation = SideViewCameraComponent->GetComponentLocation();
	auto difference = m_characterLocation - cameraLocation;
	auto stride = difference / m_timer * deltaTime;
	SideViewCameraComponent->SetWorldLocation(cameraLocation + stride);

	if (m_timer > 0)
	{
		m_timer -= deltaTime;
		if (m_timer <= 0)
			SwitchState(Stop);
	}
}

// Called every frame
void UFollowingCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (m_state)
	{
	case Stop:
		break;
	case MoveOut:
		OnMoveOutUpdate(DeltaTime);
		break;
	case Stay:
		OnStayUpdate(DeltaTime);
		break;
	case MoveBack:
		OnMoveBackUpdate(DeltaTime);
		break;
	}	
}

void UFollowingCameraComponent::SwitchState(CameraState i_state)
{
	switch (i_state)
	{
	case UFollowingCameraComponent::Stop:
		if (m_character != nullptr && SideViewCameraComponent != nullptr)
		{
			m_character->SetEnableInput(true);
			m_character = nullptr;
			SideViewCameraComponent->SetEnableCamera(true);
			SideViewCameraComponent = nullptr;
			CameraBoom = nullptr;
		}
		break;
	case UFollowingCameraComponent::MoveOut:
		m_timer = MoveOutTime;
		break;
	case UFollowingCameraComponent::Stay:
		m_timer = StayTime;
		m_OnCameraStartStay.Broadcast();
		break;
	case UFollowingCameraComponent::MoveBack:
		m_timer = MoveBackTime;
		break;
	}

	m_state = i_state;

}

void UFollowingCameraComponent::StartFollowing()
{
	auto cameraLocation = SideViewCameraComponent->GetComponentLocation();
	auto difference = m_targetLocation - cameraLocation;
	if (abs(difference.Y) > 1400.0f || abs(difference.Z) > 800.0f)
	{
		SwitchState(MoveOut);
		m_character->SetEnableInput(false);
	}
}

void UFollowingCameraComponent::DestorySelf()
{
	DestroyComponent();
}

