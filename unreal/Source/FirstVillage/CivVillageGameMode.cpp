#include "CivVillageGameMode.h"
#include "CivWorldDirector.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
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
    UWorld* World = GetWorld();
    if (!World) return;

    AStaticMeshActor* Ground = World->SpawnActor<AStaticMeshActor>(FVector(0.f, 0.f, -55.f), FRotator::ZeroRotator);
    if (!Ground) return;

    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube)
    {
        Ground->GetStaticMeshComponent()->SetStaticMesh(Cube);
        Ground->GetStaticMeshComponent()->SetWorldScale3D(FVector(65.f, 65.f, 0.5f));
        Ground->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
    }
}

void ACivVillageGameMode::SpawnNavigationBounds()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ANavMeshBoundsVolume* NavBounds = World->SpawnActor<ANavMeshBoundsVolume>(FVector(0.f, 0.f, 400.f), FRotator::ZeroRotator);
    if (!NavBounds) return;

    NavBounds->SetActorScale3D(FVector(32.f, 32.f, 8.f));
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
