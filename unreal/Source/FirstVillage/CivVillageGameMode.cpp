#include "CivVillageGameMode.h"
#include "CivWorldDirector.h"
#include "CivRuntimeEnvironment.h"
#include "CivWorldClock.h"
#include "GameFramework/DefaultPawn.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"

ACivVillageGameMode::ACivVillageGameMode()
{
    DefaultPawnClass = ADefaultPawn::StaticClass();
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
    }
}

void ACivVillageGameMode::SpawnNavigationBounds()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ANavMeshBoundsVolume* NavBounds = World->SpawnActor<ANavMeshBoundsVolume>(FVector(0.f, 0.f, 700.f), FRotator::ZeroRotator);
    if (!NavBounds) return;

    NavBounds->SetActorScale3D(FVector(42.f, 42.f, 14.f));
    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
    {
        Nav->OnNavigationBoundsUpdated(NavBounds);
    }
}

void ACivVillageGameMode::SpawnWorldDirector()
{
    if (UWorld* World = GetWorld())
    {
        World->SpawnActor<ACivWorldDirector>(FVector::ZeroVector, FRotator::ZeroRotator);
    }
}
