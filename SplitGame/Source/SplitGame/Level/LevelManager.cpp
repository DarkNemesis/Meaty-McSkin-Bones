// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelManager.h"
#include "Engine/GameEngine.h"
#include "Engine/World.h"
#include "SplitGameCharacter.h"
#include "EngineUtils.h"
#include "Components/InputComponent.h"
#include "Runtime/Engine/Public/UnrealClient.h"
#include "../UI/ScreenshotWidget.h"
#include "../UI/MapMenuWidget.h"
#include "../UI/LevelName.h"
#include "Sound/SoundWave.h"
#include "../UI/InteractionButtonWidget.h"
#include <Misc/OutputDeviceNull.h>
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Component/SFXComponent.h"
#include "UObjectIterator.h"
#include "Components/AudioComponent.h"

TArray<FString> ALevelManager::m_UnlockedLevels;

void ALevelManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	for (TActorIterator<ASplitGameCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		m_AllActorsInLevel.Add(*ActorItr);
	}
}

ALevelManager::ALevelManager()
{
	//Swtich Audio

	m_SwitchCharacterAudio = CreateDefaultSubobject<USFXComponent>(TEXT("SwitchAudio"));
	m_SwitchCharacterAudio->SetupAttachment(RootComponent);
	m_SwitchCharacterAudio->AddAudio("SoundWave'/Game/SFX/Melted_Cheese_Ambient/MMSB_OBJ_MELTED_CHEESE_BUBBLES_003.MMSB_OBJ_MELTED_CHEESE_BUBBLES_003'", USFXComponent::SFXAssetType::SoundWave);

	//End Switch Audio
}

void ALevelManager::BeginPlay()
{
	PrimaryActorTick.bCanEverTick = true;
	InputComponent->BindAction("NextCharacter", IE_Pressed, this, &ALevelManager::NextCharacter);
	InputComponent->BindAction("PreviousCharacter", IE_Pressed, this, &ALevelManager::PreviousCharacter);
	//InputComponent->BindAction("BugReport", IE_Pressed, this, &ALevelManager::GenerateBugReport);
	InputComponent->BindAction("MainMenu", IE_Pressed, this, &ALevelManager::LoadMainMenu);

	// get save system class
	UClass* result = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/BP_SaveGame.BP_SaveGame_C"));
	check(result);
	m_SaveSystemClass = result;
	// spawn save system
	m_SaveSystem = GetWorld()->SpawnActor<AActor>(m_SaveSystemClass, FTransform());


	//Background Audio
	m_BackgroundWave = LoadObject<USoundWave>(nullptr, TEXT("/Game/Audio/Final_Final_Track_Updated_April_9__1_.Final_Final_Track_Updated_April_9__1_"));

	audioComp = NewObject<UAudioComponent>(this, "Background audio");

	audioComp->RegisterComponent();
	audioComp->SetWorldLocation(FVector(0, 0, 0));
	audioComp->SetWorldRotation(FRotator(0, 0, 0));
	audioComp->SetSound(m_BackgroundWave);

	audioComp->Play(0);
	b_IsPlayedFirstTime = true;

	//End background Audio


	//Checking if the level is finished before.. So as not to push into the level array again

	m_LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	if (m_UnlockedLevels.Num() > 0)
	{
		for (auto level : m_UnlockedLevels)
		{
			if (level == m_LevelName)
			{
				m_IsLevelCompleted = true;
			}
		}
	}

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(FInputModeGameOnly());

	//Calling Begin Play here.
	Super::BeginPlay();

	//Global UI templates used to create widgets managed by level manager
	WidgetTemplate = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/UI/Screenshot.Screenshot_C"));

	MapTemplate = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/UI/BP_MapMenu.BP_MapMenu_C"));

	InteractionButtonTemplate = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/UI/BP_InteractionButton.BP_InteractionButton_C"));

	m_InteractionButtonWidget = Cast<UInteractionButtonWidget>(CreateWidget(GetWorld()->GetGameInstance(), InteractionButtonTemplate, FName("Interaction Button")));

	VictoryTemplate = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/UI/BP_Victory.BP_Victory_C"));

	PauseTemplate = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/UI/BP_PauseMenu.BP_PauseMenu_C"));

	m_PauseWidget = CreateWidget(GetWorld()->GetGameInstance(), PauseTemplate, "Pause Menu");

	LevelNameTemplate = LoadObject<UClass>(nullptr, TEXT("/Game/SideScrollerCPP/Blueprints/UI/BP_LevelName.BP_LevelName_C"));

	m_LevelNameWidget = Cast<ULevelName>(CreateWidget(GetWorld()->GetGameInstance(), LevelNameTemplate, "Level Name"));

	m_VictoryWidget = CreateWidget(GetWorld()->GetGameInstance(), VictoryTemplate, "Victory");

	if (m_LevelNameWidget)
	{
		if (!m_LevelNameWidget->IsVisible())
		{
			m_LevelNameWidget->SetTexture(m_LevelName);
			m_LevelNameWidget->AddToViewport();
			m_LevelNameShown = true;
		}
	}

	//End Widget Templates

	auto temp = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	auto t = Cast<ASplitGameCharacter>(temp);
	if (t != nullptr)
	{
		t->SetPiloting(true);
		t->SetPointer();
		m_CurrentCharacter = t;
	}
}

void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (b_IsActionButtonVisible)
	{
		m_InteractionTimer += DeltaTime;
		if (m_InteractionTimer > 4)
		{
			m_InteractionButtonWidget->RemoveFromViewport();
			b_IsActionButtonVisible = false;
			m_InteractionTimer = 0;
		}
	}

	if (b_LevelTimer)
	{
		m_LevelTimer += DeltaTime;
		if (m_LevelTimer > 10.0f)
		{
			if ((m_LevelName == "01_bones-skin") || (m_LevelName == "01_meaty-bones") || (m_LevelName == "01_meaty-skin"))
			{
				ShowLevelInteractionWidget();
				b_IsLevelInteractionButtonVisible = true;
				b_LevelTimer = false;
			}
		}
	}

	if (b_IsPlayedFirstTime)
	{
		m_BackgroundAudioTimer += DeltaTime;
		if (m_BackgroundAudioTimer > 104.f)
		{
			audioComp->Play(9.0f);
			m_BackgroundAudioTimer = 0.0f;
		}
	}

	if (m_LevelNameShown)
	{
		m_LevelInteractionTimer += DeltaTime;
		if (m_LevelInteractionTimer > 3.0f)
		{
			m_LevelNameWidget->RemoveFromViewport();
			m_LevelInteractionTimer = 0.0f;
			m_LevelNameShown = false;
		}
	}

	if (b_IsLevelInteractionButtonVisible)
	{
		m_LevelInteractionTimer += DeltaTime;
		if (m_LevelInteractionTimer > 2)
		{
			RemoveLevelInteractionWidget();
		}
	}

	m_Timer += DeltaTime;
	if (m_Timer > 1)
	{
		if (m_CurrentCharacter != nullptr)
		{
			m_CurrentCharacter->RemovePointer();
		}
		m_Timer = 0;
	}
}

//RecordScreen();



TArray<FString> ALevelManager::GetUnlockedLevels()
{
	return m_UnlockedLevels;
}


void ALevelManager::SetUnlockedLevels(TArray<FString> i_UnlockedLevels)
{
	m_UnlockedLevels = i_UnlockedLevels;
}

void ALevelManager::ShowInteractionWidget()
{
	if (!b_ShouldShowInteractionUI) return;
	if (m_InteractionButtonWidget == nullptr)
	{
		m_InteractionButtonWidget = Cast<UInteractionButtonWidget>(CreateWidget(GetWorld()->GetGameInstance(), InteractionButtonTemplate, FName("Interaction Button")));
	}
	m_InteractionButtonWidget->ShowInteractionButton(EInteractionTypes::CharacterInteraction);
	if (!m_InteractionButtonWidget->GetIsVisible())
	{
		m_InteractionButtonWidget->AddToViewport();
	}
	b_IsActionButtonVisible = true;
}

void ALevelManager::ShowInteractionWidgetForType(EInteractionTypes i_InteractionType)
{
	if (!b_ShouldShowInteractionUI) return;
	if (m_InteractionButtonWidget == nullptr)
	{
		m_InteractionButtonWidget = Cast<UInteractionButtonWidget>(CreateWidget(GetWorld()->GetGameInstance(), InteractionButtonTemplate, FName("Interaction Button")));
	}
	m_InteractionButtonWidget->ShowInteractionButton(i_InteractionType);
	if (!m_InteractionButtonWidget->GetIsVisible())
	{
		m_InteractionButtonWidget->AddToViewport();
	}
	else if(i_InteractionType == EInteractionTypes::Cancel)
	{
		m_InteractionButtonWidget->RemoveFromViewport();
		m_InteractionButtonWidget->AddToViewport();
		m_InteractionTimer = 0.0f;
	}
	b_IsActionButtonVisible = true;
}

void ALevelManager::RemoveInteractionWidget()
{
	if ((m_InteractionButtonWidget != nullptr) && (m_InteractionButtonWidget->GetIsVisible()))
	{
		m_InteractionButtonWidget->RemoveFromViewport();
		b_IsActionButtonVisible = false;
	}
}

void ALevelManager::ShowLevelInteractionWidget()
{
	if (!b_ShouldShowInteractionUI) return;
	if (m_InteractionButtonWidget == nullptr)
	{
		m_InteractionButtonWidget = Cast<UInteractionButtonWidget>(CreateWidget(GetWorld()->GetGameInstance(), InteractionButtonTemplate, FName("Interaction Button")));
	}
	m_InteractionButtonWidget->ShowInteractionButton(EInteractionTypes::SwitchInteraction);
	if (!m_InteractionButtonWidget->GetIsVisible())
	{
		m_InteractionButtonWidget->AddToViewport();
	}
	b_IsLevelInteractionButtonVisible = true;
}

void ALevelManager::RemoveLevelInteractionWidget()
{
	if (m_InteractionButtonWidget->GetIsVisible())
	{
		m_InteractionButtonWidget->RemoveFromViewport();
		b_IsLevelInteractionButtonVisible = false;
		m_LevelInteractionTimer = 0;
	}
}

void ALevelManager::ShowVictoryWidget()
{
	if (!m_VictoryWidget->IsVisible())
	{
		m_VictoryWidget->AddToViewport();
	}
}

void ALevelManager::RemoveVictoryWidget()
{
	m_VictoryWidget->RemoveFromViewport();
}

void ALevelManager::ShowActionWidget()
{
	if (!b_ShouldShowInteractionUI) return;
	m_InteractionButtonWidget = Cast<UInteractionButtonWidget>(CreateWidget(GetWorld()->GetGameInstance(), InteractionButtonTemplate, FName("Interaction Button")));
	m_InteractionButtonWidget->ShowInteractionButton(EInteractionTypes::WallUpInteraction);
	m_InteractionButtonWidget->AddToViewport();
	b_IsActionButtonVisible = true;
}

void ALevelManager::SaveCompletedLevel()
{
	if (m_IsLevelCompleted)
	{
		return;
	}
	m_IsLevelCompleted = true;
	if (m_SaveSystem)
	{
		FOutputDeviceNull ar;
		FString functionCallString = "SaveLevelMap " + UGameplayStatics::GetCurrentLevelName(GetWorld());
		m_SaveSystem->CallFunctionByNameWithArguments(*functionCallString, ar, NULL, true);
	}
	m_UnlockedLevels.Push(UGameplayStatics::GetCurrentLevelName(GetWorld()));
}

TArray<ASplitGameCharacter*> ALevelManager::GetAllActors()
{
	return m_AllActorsInLevel;
}

void ALevelManager::ShowLevelMap()
{
	for (TObjectIterator<UUserWidget> allWidgets; allWidgets; ++allWidgets)
	{
		UUserWidget* widget = *allWidgets;
		widget->RemoveFromViewport();
	}

	m_MapMenuWidget = Cast<UMapMenuWidget>(CreateWidget(GetWorld()->GetGameInstance(), MapTemplate, FName("Map")));
	if (!m_MapMenuWidget->GetIsVisible())
	{
		m_MapMenuWidget->AddToViewport();
	}
	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(m_MapMenuWidget->GetCachedWidget());
	GetWorld()->GetFirstPlayerController()->SetInputMode(Mode);
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
}

ASplitGameCharacter* ALevelManager::GetCurrentCharacter()
{
	return m_CurrentCharacter;
}

void ALevelManager::NextCharacter()
{
	if (m_AllActorsInLevel.Num() <= 1) return;
	PawnIndex = (PawnIndex + 1) % (m_AllActorsInLevel.Num());
	auto currentSelectedChar = m_AllActorsInLevel[PawnIndex];
	auto controller = currentSelectedChar->GetController();
	GetWorld()->GetFirstPlayerController()->Possess(Cast<APawn>(currentSelectedChar));
	controller->Possess(m_CurrentCharacter);

	for (auto&x : m_AllActorsInLevel)
	{
		x->SetPiloting(false);
		x->RemovePointer();
		x->OnSwitchFrom();
	}
	currentSelectedChar->SetPiloting(true);
	currentSelectedChar->SetPointer();
	currentSelectedChar->OnSwitchTo();
	//m_SwitchCharacterAudio->Play();
	m_CurrentCharacter = currentSelectedChar;
	m_Timer = 0;
}

void ALevelManager::PreviousCharacter()
{
	if (m_AllActorsInLevel.Num() <= 1) return;
	auto temp = (PawnIndex + 1);
	PawnIndex = (PawnIndex + 1) % (m_AllActorsInLevel.Num());
	auto currentSelectedChar = m_AllActorsInLevel[m_AllActorsInLevel.Num() - temp];
	auto controller = currentSelectedChar->GetController();
	GetWorld()->GetFirstPlayerController()->Possess(Cast<APawn>(currentSelectedChar));
	controller->Possess(m_CurrentCharacter);

	for (auto&x : m_AllActorsInLevel)
	{
		x->SetPiloting(false);
		x->RemovePointer();
		x->OnSwitchFrom();
	}
	currentSelectedChar->SetPiloting(true);
	currentSelectedChar->OnSwitchTo();
	currentSelectedChar->SetPointer();
	//m_SwitchCharacterAudio->Play();
	m_CurrentCharacter = currentSelectedChar;
	m_Timer = 0;
}

//void ALevelManager::BugReportHelper(int32 sizeX, int32 sizeY,const TArray<FColor>& i_Bitmap)
//{
//	UTexture2D* tempTexture = UTexture2D::CreateTransient(sizeX, sizeY);
//	if (tempTexture)
//	{
//		tempTexture->SRGB = 0;
//		FTexture2DMipMap& Mip = tempTexture->PlatformData->Mips[0];
//		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
//		int32 stride = (int32)(sizeof(uint8) * 4);
//		FMemory::Memcpy(Data, i_Bitmap.GetData(), tempTexture->PlatformData->SizeX * tempTexture->PlatformData->SizeY * stride);
//		Mip.BulkData.Unlock();
//		tempTexture->UpdateResource();
//	}
//	m_BugReportWidget = Cast<UScreenshotWidget>(CreateWidget(GetWorld()->GetGameInstance(), WidgetTemplate, FName("Screenshot")));
//	m_BugReportWidget->m_ScreenshotTexture = tempTexture;
//	m_BugReportWidget->bitMapData = i_Bitmap;
//	m_BugReportWidget->X = sizeX;
//	m_BugReportWidget->Y = sizeY;
//	if (!m_BugReportWidget->GetIsVisible())
//	{
//		m_BugReportWidget->AddToViewport();
//	}
//	FInputModeUIOnly Mode;
//	Mode.SetWidgetToFocus(m_BugReportWidget->GetCachedWidget());
//	GetWorld()->GetFirstPlayerController()->SetInputMode(Mode);
//	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
//}
//
//void ALevelManager::GenerateBugReport()
//{
//	GetWorld()->GetGameViewport()->OnScreenshotCaptured().AddUObject(this, &ALevelManager::BugReportHelper);
//	FScreenshotRequest::RequestScreenshot(false);
//}

void ALevelManager::LoadMainMenu()
{
	FLatentActionInfo latentInfo;
	//UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", true, "PauseMenu");

	if (!m_PauseWidget->GetIsVisible())
	{
		m_PauseWidget->AddToViewport();
	}
}