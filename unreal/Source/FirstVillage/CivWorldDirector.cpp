#include "CivWorldDirector.h"
#include "CivAgentCharacter.h"
#include "CivAgentMindComponent.h"
#include "CivAIController.h"
#include "CivResourceNode.h"
#include "CivProceduralBuilding.h"
#include "CivRuntimeEnvironment.h"
#include "Kismet/GameplayStatics.h"
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

void ACivWorldDirector::AddHarvest(ECivResourceType Type, float Amount)
{
    if (Amount <= 0.f) return;
    switch (Type)
    {
        case ECivResourceType::Water: WaterStore += Amount; break;
        case ECivResourceType::Food:
        case ECivResourceType::Game: FoodStore += Amount; break;
        case ECivResourceType::Wood: WoodStore += Amount; break;
        case ECivResourceType::Stone: StoneStore += Amount; break;
        case ECivResourceType::Clay: ClayStore += Amount; break;
    }
}

float ACivWorldDirector::GetStoreAmount(FName Resource) const
{
    if (Resource == TEXT("food")) return FoodStore;
    if (Resource == TEXT("water")) return WaterStore;
    if (Resource == TEXT("wood")) return WoodStore;
    if (Resource == TEXT("stone")) return StoneStore;
    if (Resource == TEXT("clay")) return ClayStore;
    if (Resource == TEXT("thatch")) return ThatchStore;
    return 0.f;
}

void ACivWorldDirector::GetMaterialStores(TMap<FName, float>& OutStores) const
{
    OutStores.Reset();
    OutStores.Add(TEXT("wood"), WoodStore);
    OutStores.Add(TEXT("stone"), StoneStore);
    OutStores.Add(TEXT("clay"), ClayStore);
    OutStores.Add(TEXT("thatch"), ThatchStore);
}

bool ACivWorldDirector::ConsumeMaterials(const TMap<FName, float>& Costs)
{
    for (const TPair<FName, float>& Pair : Costs)
    {
        if (GetStoreAmount(Pair.Key) < Pair.Value)
        {
            return false;
        }
    }

    for (const TPair<FName, float>& Pair : Costs)
    {
        if (Pair.Key == TEXT("wood")) WoodStore -= Pair.Value;
        else if (Pair.Key == TEXT("stone")) StoneStore -= Pair.Value;
        else if (Pair.Key == TEXT("clay")) ClayStore -= Pair.Value;
        else if (Pair.Key == TEXT("thatch")) ThatchStore -= Pair.Value;
    }
    return true;
}

ACivAgentCharacter* ACivWorldDirector::FindAgentById(int32 AgentId) const
{
    for (ACivAgentCharacter* Agent : Agents)
    {
        if (Agent && Agent->Mind && Agent->Mind->AgentId == AgentId)
        {
            return Agent;
        }
    }
    return nullptr;
}

FVector ACivWorldDirector::ProjectToGround(const FVector& DesiredLocation, float HeightOffset) const
{
    if (UWorld* World = GetWorld())
    {
        if (ACivRuntimeEnvironment* Environment = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(World, ACivRuntimeEnvironment::StaticClass())))
        {
            return FVector(DesiredLocation.X, DesiredLocation.Y, Environment->GetTerrainZAtWorldXY(DesiredLocation.X, DesiredLocation.Y) + HeightOffset);
        }

        FHitResult Hit;
        const FVector Start(DesiredLocation.X, DesiredLocation.Y, 5000.f);
        const FVector End(DesiredLocation.X, DesiredLocation.Y, -5000.f);
        FCollisionQueryParams Params(SCENE_QUERY_STAT(FirstVillageGroundTrace), false, this);
        if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
        {
            return Hit.ImpactPoint + FVector(0.f, 0.f, HeightOffset);
        }
    }

    return DesiredLocation;
}

void ACivWorldDirector::SpawnInitialPopulation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    static const TCHAR* Names[] = {
        TEXT("Mara"), TEXT("Eli"), TEXT("June"), TEXT("Caleb"), TEXT("Nora"), TEXT("Theo"),
        TEXT("Iris"), TEXT("Jonah"), TEXT("Mae"), TEXT("Rowan"), TEXT("Ada"), TEXT("Silas")
    };
    static const FName Roles[] = {
        TEXT("Forager"), TEXT("Maker"), TEXT("Healer"), TEXT("Scout"), TEXT("Cook"), TEXT("Gatherer"),
        TEXT("Farmer"), TEXT("Builder"), TEXT("Healer"), TEXT("Hunter"), TEXT("Maker"), TEXT("Organizer")
    };
    static const FName Traits[] = {
        TEXT("cooperative"), TEXT("cautious"), TEXT("empathetic"), TEXT("bold"), TEXT("practical"), TEXT("curious"),
        TEXT("stubborn"), TEXT("calm"), TEXT("social"), TEXT("competitive"), TEXT("inventive"), TEXT("organized")
    };

    for (int32 i = 0; i < InitialPopulation; ++i)
    {
        const float Angle = (2.f * PI * i) / FMath::Max(1, InitialPopulation);
        FVector Location = GetActorLocation() + FVector(FMath::Cos(Angle) * SpawnRadius * 0.35f, FMath::Sin(Angle) * SpawnRadius * 0.35f, 0.f);
        Location = ProjectToGround(Location, 105.f);

        ACivAgentCharacter* Agent = World->SpawnActor<ACivAgentCharacter>(Location, FRotator::ZeroRotator);
        if (!Agent) continue;

        if (Agent->Mind)
        {
            Agent->Mind->AgentId = i;
            Agent->Mind->DisplayName = Names[i % UE_ARRAY_COUNT(Names)];
            Agent->Mind->Role = Roles[i % UE_ARRAY_COUNT(Roles)];
            Agent->Mind->Trait = Traits[i % UE_ARRAY_COUNT(Traits)];
            Agent->Mind->PrivateGoal = TEXT("Help the group survive and make a lasting home in the valley.");

            FCivMemory FoundingMemory;
            FoundingMemory.Text = TEXT("We entered this valley together with little more than what we could carry.");
            FoundingMemory.Source = TEXT("firsthand");
            FoundingMemory.Confidence = 1.f;
            FoundingMemory.DayCreated = 1;
            FoundingMemory.bFirsthand = true;
            Agent->Mind->AddMemory(FoundingMemory);
        }

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
        FVector Location = GetActorLocation() + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);
        Location = ProjectToGround(Location, 55.f);

        ACivResourceNode* Node = World->SpawnActor<ACivResourceNode>(Location, FRotator::ZeroRotator);
        if (!Node) continue;
        Node->ResourceType = Types[i % UE_ARRAY_COUNT(Types)];
        Node->Quantity = Node->ResourceType == ECivResourceType::Water ? 10000.f : 150.f;
        Node->bRenewable = Node->ResourceType == ECivResourceType::Water || Node->ResourceType == ECivResourceType::Food || Node->ResourceType == ECivResourceType::Game;
        Node->RefreshVisual();
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
        TArray<FName> Primitives;
    };

    const FDemo Demo[] = {
        { FVector(450.f, 150.f, 0.f), TEXT("First Hut"), TEXT("housing"), FVector2D(8.f, 8.f), { TEXT("walls"), TEXT("roof"), TEXT("posts"), TEXT("doorway") } },
        { FVector(-420.f, 220.f, 0.f), TEXT("Smokehouse"), TEXT("preserve meat"), FVector2D(8.f, 8.f), { TEXT("walls"), TEXT("roof"), TEXT("hearth"), TEXT("drying_beams"), TEXT("doorway") } },
        { FVector(0.f, -520.f, 0.f), TEXT("Meeting Shelter"), TEXT("gathering and decisions"), FVector2D(10.f, 8.f), { TEXT("posts"), TEXT("roof"), TEXT("hearth"), TEXT("platform") } }
    };

    for (const FDemo& D : Demo)
    {
        const FVector Location = ProjectToGround(GetActorLocation() + D.Offset, 4.f);
        ACivProceduralBuilding* Building = World->SpawnActor<ACivProceduralBuilding>(Location, FRotator::ZeroRotator);
        if (!Building) continue;
        Building->Blueprint.Name = D.Name;
        Building->Blueprint.Purpose = D.Purpose;
        Building->Blueprint.Footprint = D.Footprint;
        Building->Blueprint.Primitives = D.Primitives;
        Building->RebuildFromBlueprint();
    }
}
