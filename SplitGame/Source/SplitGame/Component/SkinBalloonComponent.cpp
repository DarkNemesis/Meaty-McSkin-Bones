// Fill out your copyright notice in the Description page of Project Settings.

#include "SkinBalloonComponent.h"
#include "../SkinCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/GameEngine.h"
#include "Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


// Sets default values for this component's properties
USkinBalloonComponent::USkinBalloonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkinBalloonComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void USkinBalloonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (b_StartSwelling)
	{
		m_LocalTimer += DeltaTime;
		if (m_LocalTimer > 0.025f)
		{
			Swell();
		}
	}
	else if (b_StartReleasing)
	{
		m_LocalTimer += DeltaTime;
		if (m_LocalTimer > 1)
		{
			Release();
		}
	}
}

void USkinBalloonComponent::SetDefaultValues(ASkinCharacter* i_Parent, AMuscleCharacter* i_Muscle)
{
	m_ParentSkin = i_Parent;
	m_CallingMuscle = i_Muscle;
}

void USkinBalloonComponent::Launch()
{
	b_StartSwelling = true;
}

void USkinBalloonComponent::Cancel()
{
	m_AmountOfAirHeld = 0;
	//m_ParentSkin->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	b_StartReleasing = true;
	b_StartSwelling = false;
}

void USkinBalloonComponent::Swell()
{
	m_LocalTimer = 0.0f;
	if (m_AmountOfAirHeld < m_MaxAmountOfAirHeld)
	{
		m_ParentSkin->SetGravityScale(0.0f);
		if (m_ParentSkin->m_CurrentSkinState != eSkinStates::Balloon)
		{
			m_ParentSkin->m_CurrentSkinState = eSkinStates::Balloon;
		}
		//m_ParentSkin->SetActorLocation(FMath::Lerp(m_ParentSkin->GetActorLocation(), m_ParentSkin->GetActorLocation() + FVector(0, 0, m_AmountOfAirPerTick), 1.0f));
		m_ParentSkin->GetCharacterMovement()->AddForce(m_AmountOfAirHeld * FVector::UpVector * 4.5f);
		m_AmountOfAirHeld += m_AmountOfAirPerTick;
	}
	else
	{
		m_ParentSkin->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		b_StartSwelling = false;
		b_StartReleasing = true;
		m_LocalTimer = 0.0f;
	}
}

void USkinBalloonComponent::Release()
{
	if (m_ParentSkin->IsValidLowLevel())
	{
		if (m_AmountOfAirHeld > 0)
		{
			if (m_ParentSkin->InputComponent->IsValidLowLevel())
			{
				if (m_ParentSkin->InputComponent->GetAxisValue("Walk") != 0)
				{
					FVector vector = m_ParentSkin->InputComponent->GetAxisValue("Walk") > 0 ? FVector(0, 1, 0) : FVector(0, -1, 0);
					FVector totalAmountOfImpulse = m_ParentSkin->GetInputState() ? vector * -m_MaxAmountOfAirReleased * m_MaxForceOfAirReleased : vector * -m_DefaultAmountOfAirReleased * m_DefaultForceOfAirReleased;
					m_ParentSkin->GetCharacterMovement()->AddImpulse(totalAmountOfImpulse);
					m_AmountOfAirHeld -= m_ParentSkin->GetInputState() ? m_MaxAmountOfAirReleased / m_FallRate : m_DefaultAmountOfAirReleased / m_FallRate;
					if (m_AmountOfAirHeld > 0) { m_ParentSkin->SetGravityScale(2.0f / (m_AmountOfAirHeld * m_FallRate)); }
				}
				else
				{
					m_AmountOfAirHeld -= m_ParentSkin->GetInputState() ? m_MaxAmountOfAirReleased / m_FallRate : m_DefaultAmountOfAirReleased / m_FallRate;
					if (m_AmountOfAirHeld > 0) { m_ParentSkin->SetGravityScale(2.0f / (m_AmountOfAirHeld * m_FallRate)); }
				}
			}
			else
			{
				m_AmountOfAirHeld -= m_ParentSkin->GetInputState() ? m_MaxAmountOfAirReleased / m_FallRate : m_DefaultAmountOfAirReleased / m_FallRate;
				if (m_AmountOfAirHeld > 0) { m_ParentSkin->SetGravityScale(2.0f / (m_AmountOfAirHeld * m_FallRate)); }
			}
		}
		else if (m_ParentSkin->m_CurrentSkinState != eSkinStates::Skin)
		{
			m_ParentSkin->SetGravityScale(2.0f);
			m_ParentSkin->m_CurrentSkinState = eSkinStates::Skin;
			m_AmountOfAirHeld = 0.0f;
			b_StartReleasing = false;
			m_LocalTimer = 0.0f;
		}
	}
}



