#include "CivVillageGameMode.h"
#include "CivWorldDirector.h"
#include "CivRuntimeEnvironment.h"
#include "CivWorldClock.h"
#include "CivBiomeDresser.h"
#include "CivObserverHUD.h"
#include "CivObserverPawn.h"
#include "CivRiverActor.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"

ACivVillageGameMode::ACivVillageGameMode()
{
    DefaultPawnClass = ACivObserverPawn::StaticClass();
    HUDClass = ACivObserverHUD::StaticClass();
}

void ACivVillageGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnRuntimeGround();
    SpawnNavigationBounds();
    SpawnWorldDirector();
}

void ACivVillageGameMode::SpawnRuntimeGround()
{
    if (UWorld* World = GetWorld())
    {
        World->SpawnActor<ACivRuntimeEnvironment>(FVector::ZeroVector, FRotator::ZeroRotator);
        World->SpawnActor<ACivWorldClock>(FVector::ZeroVector, FRotator::ZeroRotator);
        World->SpawnActor<ACivBiomeDresser>(FVector::ZeroVector, FRotator::ZeroRotator);
        World->SpawnActor<ACivRiverActor>(FVector::ZeroVector, FRotator::ZeroRotator);
    }
}

void ACivVillageGameMode::SpawnNavigationBounds()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ANavMeshBoundsVolume* NavBounds = World->SpawnActor<ANavMeshBoundsVolume>(FVector(0.f, 0.f, 550.f), FRotator::ZeroRotator);
    if (!NavBounds) return;

    NavBounds->SetActorScale3D(FVector(48.f, 48.f, 12.f));
    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
    {
        Nav->OnNavigationBoundsUpdated(NavBounds);
        Nav->Build();
    }
}

void ACivVillageGameMode::SpawnWorldDirector()
{
    if (UWorld* World = GetWorld())
    {
        World->SpawnActor<ACivWorldDirector>(FVector::ZeroVector, FRotator::ZeroRotator);
    }
}
