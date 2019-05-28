#include "EnvFlag.h"
#include "SplitGameCharacter.h"
#include "Runtime/Engine/Classes/Engine/TriggerVolume.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Level/LevelManager.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Component/SFXComponent.h"
#include "SkinCharacter.h"

AEnvFlag::AEnvFlag()
{
	PrimaryActorTick.bCanEverTick = true;

	// Our root component will be a box that reacts to physics
	UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = BoxComponent;
	BoxComponent->InitBoxExtent(FVector(80.0f, 100.f, 100.f));
	BoxComponent->SetCollisionProfileName(TEXT("EnvFlag"));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnvFlag::OnInteractiveBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEnvFlag::OnInteractiveEnd);		
	
	timerDelegate.BindUFunction(this, "OpenMenu");

	SFXComponent = CreateDefaultSubobject<USFXComponent>(TEXT("SFXComponent"));
	SFXComponent->AddAudio("SoundWave'/Game/SFX/Level_Complete/MMSB_EVENT_1FINISH_C03__1_.MMSB_EVENT_1FINISH_C03__1_'", USFXComponent::SoundWave);
}

void AEnvFlag::BeginPlay()
{
	Super::BeginPlay();

	m_CheeseWidgetActor = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/Victory/BP_Victory_Cheese.BP_Victory_Cheese_C"));
}

void AEnvFlag::Tick(float DeltaTime)
{
	if (mCharacters.Num() >= mCharactersRequired && !GetWorldTimerManager().TimerExists(timerHandle))
	{
		ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
		if (levelManager)
		{
			isFinished = true;
			levelManager->SaveCompletedLevel();
			SFXComponent->Play();
			// freeze characters
			auto actors = levelManager->GetAllActors();
			for (auto actor : actors)
			{
				actor->SetEnableInput(false);
				if (auto skin = Cast<ASkinCharacter>(actor))
				{
					skin->PlayCelebrationAudio();
				}
			}
			levelManager->ShowVictoryWidget();
			FActorSpawnParameters spawnInfo;
			auto pos = GetActorLocation();
			GetWorld()->SpawnActor<AActor>(m_CheeseWidgetActor, FVector(pos.X, pos.Y, pos.Z - 50), FRotator(0, 0, 0), spawnInfo);
			GetWorldTimerManager().SetTimer(timerHandle, timerDelegate, timer, false);
			//UGameplayStatics::OpenLevel((UObject*)GetWorld(), "MainMenu", true, "MapMenu");
		}
	}
}

void AEnvFlag::OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (auto character = Cast<ASplitGameCharacter>(OtherActor))
	{
		mCharacters.Add(character);
	}
}

void AEnvFlag::OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (auto character = Cast<ASplitGameCharacter>(OtherActor))
	{
		mCharacters.Remove(character);
	}
}

void AEnvFlag::OpenMenu()
{
	ALevelManager* levelManager = Cast<ALevelManager>(GetWorld()->GetLevelScriptActor());
	if(levelManager != nullptr)levelManager->RemoveVictoryWidget();
	UGameplayStatics::OpenLevel((UObject*)GetWorld(), "MainMenu", true, "MapMenu");
}





