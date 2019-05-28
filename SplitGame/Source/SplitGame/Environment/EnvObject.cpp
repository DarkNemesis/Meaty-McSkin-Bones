#include "EnvObject.h"

AEnvObject::AEnvObject()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnvObject::BeginPlay()
{
	Super::BeginPlay();	
	Refresh();
}

void AEnvObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//auto actorLocation = GetActorLocation();
	//actorLocation.X = 0;
	//SetActorLocation(actorLocation);
	
}

void AEnvObject::Action(ASplitGameCharacter* Character)
{}

void AEnvObject::OnInteractiveBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{}

void AEnvObject::OnInteractiveEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{}

void AEnvObject::Refresh()
{}

#if WITH_EDITOR
void AEnvObject::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Refresh();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
void AEnvObject::PostLoad()
{
	Refresh();
	Super::PostLoad();
}
#endif

