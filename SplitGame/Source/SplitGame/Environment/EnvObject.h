#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvObject.generated.h"

class ASplitGameCharacter;

UCLASS()
class SPLITGAME_API AEnvObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnvObject();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;	

	virtual void Action(ASplitGameCharacter* Character);

	virtual void Refresh();

	UFUNCTION()
		virtual void OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
		virtual void OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;

	virtual void PostLoad() override;
#endif
};
