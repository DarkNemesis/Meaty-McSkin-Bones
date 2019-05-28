// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FollowingCameraComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCameraStartStayDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPLITGAME_API UFollowingCameraComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFollowingCameraComponent();

	UFUNCTION(BlueprintCallable)
	void SetCamera(class ASplitGameCharacter* i_currentCharacter, class AActor* i_target);

	void SetTargetLocation(FVector i_location);

	enum CameraState
	{
		Stop,
		MoveOut,
		Stay,
		MoveBack,
	};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	CameraState m_state;

	FVector m_targetLocation;
	FVector m_characterLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USplitCameraComponent* SideViewCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	void OnMoveOutUpdate(float deltaTime);
	void OnStayUpdate(float deltaTime);
	void OnMoveBackUpdate(float deltaTime);

	float m_timer;

	ASplitGameCharacter* m_character;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float MoveOutTime;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float StayTime;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float MoveBackTime;

	UPROPERTY(EditAnywhere, Category = "Bullet Attributes")
	float MaxMoveSpeed;

	void SwitchState(CameraState i_state);

	UFUNCTION(BlueprintCallable)
	void StartFollowing();

	UFUNCTION(BlueprintCallable)
	void DestorySelf();

	UPROPERTY(BlueprintAssignable, Category = "FallowingCameraComponent", meta = (DisplayName = "OnCameraStartStay"))
	FCameraStartStayDelegate m_OnCameraStartStay;
	
};
