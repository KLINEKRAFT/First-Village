#include "CivWorldDirector.h"
#include "CivAgentCharacter.h"
#include "CivAgentMindComponent.h"
#include "CivAIController.h"
#include "CivResourceNode.h"
#include "CivProceduralBuilding.h"
#include "CivRuntimeEnvironment.h"
#include "CivHistorySubsystem.h"
#include "CivWorldClock.h"
#include "CivCampBeacon.h"
#include "CivTrailNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

namespace
{
int32 GetCurrentDay(UWorld* World)
{
    if (!World) return 1;
    if (ACivWorldClock* Clock = Cast<ACivWorldClock>(UGameplayStatics::GetActorOfClass(World, ACivWorldClock::StaticClass()))) return Clock->Day;
    return 1;
}

void RecordHistory(UWorld* World, FName Type, const FString& Summary, const FVector& Location, const TArray<int32>& AgentIds = {})
{
    if (!World || !World->GetGameInstance()) return;
    if (UCivHistorySubsystem* History = World->GetGameInstance()->GetSubsystem<UCivHistorySubsystem>())
    {
        FCivHistoryEvent Entry;
        Entry.Day = GetCurrentDay(World);
        Entry.Type = Type;
        Entry.Summary = Summary;
        Entry.WorldLocation = Location;
        Entry.AgentIds = AgentIds;
        History->RecordEvent(Entry);
    }
}
}

ACivWorldDirector::ACivWorldDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACivWorldDirector::BeginPlay()
{
    Super::BeginPlay();
    RecordHistory(GetWorld(), TEXT("founding"), TEXT("Twelve people entered the valley and established the first camp."), GetActorLocation());
    SpawnInitialPopulation();
    UE_LOG(LogTemp, Display, TEXT("First Village population spawned: %d/%d"), Agents.Num(), InitialPopulation);
    SpawnResourceRing();
    if (bGenerateDemoVillage) SpawnDemoStructures();
    SpawnVillageLandmarks();
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
        if (GetStoreAmount(Pair.Key) < Pair.Value) return false;
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
        if (Agent && Agent->Mind && Agent->Mind->AgentId == AgentId) return Agent;
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
        if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params)) return Hit.ImpactPoint + FVector(0.f, 0.f, HeightOffset);
    }
    return DesiredLocation;
}

void ACivWorldDirector::SpawnInitialPopulation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    static const TCHAR* Names[] = { TEXT("Mara"),TEXT("Eli"),TEXT("June"),TEXT("Caleb"),TEXT("Nora"),TEXT("Theo"),TEXT("Iris"),TEXT("Jonah"),TEXT("Mae"),TEXT("Rowan"),TEXT("Ada"),TEXT("Silas") };
    static const FName Roles[] = { TEXT("Forager"),TEXT("Maker"),TEXT("Healer"),TEXT("Scout"),TEXT("Cook"),TEXT("Gatherer"),TEXT("Farmer"),TEXT("Builder"),TEXT("Healer"),TEXT("Hunter"),TEXT("Maker"),TEXT("Organizer") };
    static const FName Traits[] = { TEXT("cooperative"),TEXT("cautious"),TEXT("empathetic"),TEXT("bold"),TEXT("practical"),TEXT("curious"),TEXT("stubborn"),TEXT("calm"),TEXT("social"),TEXT("competitive"),TEXT("inventive"),TEXT("organized") };

    FActorSpawnParameters AgentSpawnParams;
    AgentSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    FActorSpawnParameters ControllerSpawnParams;
    ControllerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (int32 i = 0; i < InitialPopulation; ++i)
    {
        const float Angle = (2.f * PI * i) / FMath::Max(1, InitialPopulation);
        const float Radius = 185.f + (i % 3) * 55.f;
        FVector Location = ProjectToGround(GetActorLocation() + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f), 125.f);
        ACivAgentCharacter* Agent = World->SpawnActor<ACivAgentCharacter>(ACivAgentCharacter::StaticClass(), Location, FRotator::ZeroRotator, AgentSpawnParams);
        if (!Agent)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn First Village agent %d at %s"), i, *Location.ToString());
            continue;
        }

        if (Agent->Mind)
        {
            Agent->Mind->AgentId = i;
            Agent->Mind->DisplayName = Names[i % UE_ARRAY_COUNT(Names)];
            Agent->Mind->Role = Roles[i % UE_ARRAY_COUNT(Roles)];
            Agent->Mind->Trait = Traits[i % UE_ARRAY_COUNT(Traits)];
            Agent->Mind->PrivateGoal = TEXT("Help the group survive and make a lasting home in the valley.");
            FCivMemory M;
            M.Text = TEXT("We entered this valley together with little more than what we could carry.");
            M.Source = TEXT("firsthand");
            M.Confidence = 1.f;
            M.DayCreated = 1;
            M.bFirsthand = true;
            Agent->Mind->AddMemory(M);
        }
        Agent->RefreshVisualIdentity();

        ACivAIController* Controller = World->SpawnActor<ACivAIController>(ACivAIController::StaticClass(), Agent->GetActorLocation(), FRotator::ZeroRotator, ControllerSpawnParams);
        if (Controller) Controller->Possess(Agent);
        else UE_LOG(LogTemp, Warning, TEXT("Agent %d spawned but AI controller did not."), i);
        Agents.Add(Agent);
    }
}

void ACivWorldDirector::SpawnResourceRing()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ACivRuntimeEnvironment* Env = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(World, ACivRuntimeEnvironment::StaticClass()));
    const float RiverX = Env ? Env->GetRiverCenterX() : -2200.f;
    const ECivResourceType Types[] = { ECivResourceType::Water, ECivResourceType::Food, ECivResourceType::Wood, ECivResourceType::Stone, ECivResourceType::Clay, ECivResourceType::Game };

    for (int32 i = 0; i < 18; ++i)
    {
        const ECivResourceType Type = Types[i % UE_ARRAY_COUNT(Types)];
        const int32 Group = i / 6;
        FVector Location = GetActorLocation();

        if (Type == ECivResourceType::Water)
        {
            const float Y = -1700.f + Group * 1700.f;
            Location = FVector(RiverX + FMath::Sin(Y * 0.0011f) * 150.f, Y, 0.f);
        }
        else if (Type == ECivResourceType::Clay)
        {
            const float Y = -1350.f + Group * 1350.f;
            Location = FVector(RiverX + 520.f, Y, 0.f);
        }
        else
        {
            const float BaseAngle = (2.f * PI * i) / 18.f;
            float Radius = 2100.f;
            if (Type == ECivResourceType::Wood) Radius = 2650.f;
            if (Type == ECivResourceType::Stone) Radius = 3100.f;
            if (Type == ECivResourceType::Game) Radius = 2850.f;
            Location += FVector(FMath::Cos(BaseAngle) * Radius, FMath::Sin(BaseAngle) * Radius, 0.f);
        }

        Location = ProjectToGround(Location, Type == ECivResourceType::Water ? 18.f : 55.f);
        ACivResourceNode* Node = World->SpawnActor<ACivResourceNode>(Location, FRotator::ZeroRotator);
        if (!Node) continue;
        Node->ResourceType = Type;
        Node->Quantity = Type == ECivResourceType::Water ? 10000.f : 150.f;
        Node->bRenewable = Type == ECivResourceType::Water || Type == ECivResourceType::Food || Type == ECivResourceType::Game;
        Node->RefreshVisual();
        Resources.Add(Node);
    }
}

void ACivWorldDirector::SpawnDemoStructures()
{
    UWorld* World = GetWorld();
    if (!World) return;

    struct FDemo { FVector Offset; const TCHAR* Name; const TCHAR* Purpose; FVector2D Footprint; TArray<FName> Primitives; };
    const FDemo Demo[] = {
        { FVector(500,180,0), TEXT("First Hut"), TEXT("housing"), FVector2D(8,8), { TEXT("walls"),TEXT("roof"),TEXT("posts"),TEXT("doorway") } },
        { FVector(-500,240,0), TEXT("Smokehouse"), TEXT("preserve meat"), FVector2D(8,8), { TEXT("walls"),TEXT("roof"),TEXT("hearth"),TEXT("drying_beams"),TEXT("doorway") } },
        { FVector(0,-590,0), TEXT("Meeting Shelter"), TEXT("gathering and decisions"), FVector2D(10,8), { TEXT("posts"),TEXT("roof"),TEXT("hearth"),TEXT("platform") } }
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
        RecordHistory(World, TEXT("construction"), FString::Printf(TEXT("The settlement completed %s."), D.Name), Location);
    }
}

void ACivWorldDirector::SpawnVillageLandmarks()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ACivRuntimeEnvironment* Env = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(World, ACivRuntimeEnvironment::StaticClass()));
    if (!Env) return;

    const FVector Camp = ProjectToGround(GetActorLocation(), 5.f);
    World->SpawnActor<ACivCampBeacon>(Camp, FRotator::ZeroRotator);

    ACivTrailNetwork* Trails = World->SpawnActor<ACivTrailNetwork>(FVector::ZeroVector, FRotator::ZeroRotator);
    if (!Trails) return;

    const FVector Hut = ProjectToGround(GetActorLocation() + FVector(500.f, 180.f, 0.f), 6.f);
    const FVector Smokehouse = ProjectToGround(GetActorLocation() + FVector(-500.f, 240.f, 0.f), 6.f);
    const FVector Meeting = ProjectToGround(GetActorLocation() + FVector(0.f, -590.f, 0.f), 6.f);
    const FVector RiverFord = ProjectToGround(FVector(Env->GetRiverCenterX(), 0.f, 0.f), 6.f);

    Trails->AddTrail(Camp, Hut, Env, 110.f);
    Trails->AddTrail(Camp, Smokehouse, Env, 110.f);
    Trails->AddTrail(Camp, Meeting, Env, 130.f);
    Trails->AddTrail(Camp, RiverFord, Env, 100.f);
}
