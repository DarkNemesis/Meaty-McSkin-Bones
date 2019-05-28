// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkinBalloonComponent.generated.h"

class ASkinCharacter;
class AMuscleCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLITGAME_API USkinBalloonComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	//Public variables for Ballon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		bool b_IsSecondInteractionEnabled = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		float m_MaxAmountOfAirHeld = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		float m_AmountOfAirPerTick = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		float m_DefaultAmountOfAirReleased = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		float m_MaxAmountOfAirReleased = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		float m_DefaultForceOfAirReleased = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		float m_MaxForceOfAirReleased = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balloon Properties")
		float m_FallRate = 30.0f;

public:	
	// Sets default values for this component's properties
	USkinBalloonComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetDefaultValues(ASkinCharacter* i_Parent, AMuscleCharacter* i_Muscle);
	void Launch();

	void Cancel();

private:
	ASkinCharacter* m_ParentSkin;
	AMuscleCharacter* m_CallingMuscle;
	bool b_StartSwelling = false;
	bool b_StartReleasing = false;
	float m_AmountOfAirHeld = 0.0f;
	float m_LocalTimer = 0.0f;
private:
	void Swell();
	void Release();
};
