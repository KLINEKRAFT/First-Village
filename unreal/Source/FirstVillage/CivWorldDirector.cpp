#include "CivWorldDirector.h"
#include "CivAgentCharacter.h"
#include "CivAIController.h"
#include "CivResourceNode.h"
#include "CivProceduralBuilding.h"
#include "Engine/World.h"

ACivWorldDirector::ACivWorldDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACivWorldDirector::BeginPlay()
{
    Super::BeginPlay();
    SpawnInitialPopulation();
    SpawnResourceRing();
    if (bGenerateDemoVillage)
    {
        SpawnDemoStructures();
    }
}

void ACivWorldDirector::SpawnInitialPopulation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (int32 i = 0; i < InitialPopulation; ++i)
    {
        const float Angle = (2.f * PI * i) / FMath::Max(1, InitialPopulation);
        const FVector Offset(FMath::Cos(Angle) * SpawnRadius * 0.35f, FMath::Sin(Angle) * SpawnRadius * 0.35f, 120.f);
        ACivAgentCharacter* Agent = World->SpawnActor<ACivAgentCharacter>(GetActorLocation() + Offset, FRotator::ZeroRotator);
        if (!Agent) continue;

        ACivAIController* Controller = World->SpawnActor<ACivAIController>(Agent->GetActorLocation(), FRotator::ZeroRotator);
        if (Controller)
        {
            Controller->Possess(Agent);
        }
        Agents.Add(Agent);
    }
}

void ACivWorldDirector::SpawnResourceRing()
{
    UWorld* World = GetWorld();
    if (!World) return;

    const ECivResourceType Types[] = {
        ECivResourceType::Water,
        ECivResourceType::Food,
        ECivResourceType::Wood,
        ECivResourceType::Stone,
        ECivResourceType::Clay,
        ECivResourceType::Game
    };

    for (int32 i = 0; i < 18; ++i)
    {
        const float Angle = (2.f * PI * i) / 18.f;
        const float Radius = 1700.f + 350.f * (i % 3);
        const FVector Location = GetActorLocation() + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 70.f);
        ACivResourceNode* Node = World->SpawnActor<ACivResourceNode>(Location, FRotator::ZeroRotator);
        if (!Node) continue;
        Node->ResourceType = Types[i % UE_ARRAY_COUNT(Types)];
        Node->Quantity = Node->ResourceType == ECivResourceType::Water ? 10000.f : 150.f;
        Node->bRenewable = Node->ResourceType == ECivResourceType::Water || Node->ResourceType == ECivResourceType::Food || Node->ResourceType == ECivResourceType::Game;
        Resources.Add(Node);
    }
}

void ACivWorldDirector::SpawnDemoStructures()
{
    UWorld* World = GetWorld();
    if (!World) return;

    struct FDemo
    {
        FVector Offset;
        const TCHAR* Name;
        const TCHAR* Purpose;
        FVector2D Footprint;
        bool Hearth;
    };

    const FDemo Demo[] = {
        { FVector(450.f, 150.f, 0.f), TEXT("First Hut"), TEXT("housing"), FVector2D(500.f, 400.f), false },
        { FVector(-420.f, 220.f, 0.f), TEXT("Smokehouse"), TEXT("preserve meat"), FVector2D(420.f, 340.f), true },
        { FVector(0.f, -520.f, 0.f), TEXT("Meeting Shelter"), TEXT("gathering and decisions"), FVector2D(700.f, 520.f), true }
    };

    for (const FDemo& D : Demo)
    {
        ACivProceduralBuilding* Building = World->SpawnActor<ACivProceduralBuilding>(GetActorLocation() + D.Offset, FRotator::ZeroRotator);
        if (!Building) continue;
        Building->Blueprint.Name = D.Name;
        Building->Blueprint.Purpose = D.Purpose;
        Building->Blueprint.Footprint = D.Footprint;
        Building->Blueprint.bHasHearth = D.Hearth;
        Building->RebuildFromBlueprint();
    }
}
