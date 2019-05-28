#pragma once
#include "CoreMinimal.h"
#include "Environment/EnvObject.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "EnvFlag.generated.h"

class ASplitGameCharacter;

UCLASS()
class SPLITGAME_API AEnvFlag : public AEnvObject
{
	GENERATED_BODY()

	TSet<ASplitGameCharacter*> mCharacters;

public:
	AEnvFlag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName mNextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int mCharactersRequired;

	UPROPERTY()
		UClass* m_CheeseWidgetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isFinished = false;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	float timer = 3.0f;
	FTimerDelegate timerDelegate;
	FTimerHandle timerHandle;

	UFUNCTION()
		void OpenMenu();
	class USFXComponent* SFXComponent;
};
