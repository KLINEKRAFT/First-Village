#include "CivObserverPawn.h"
#include "CivWorldDirector.h"
#include "CivAgentCharacter.h"
#include "CivRuntimeEnvironment.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

ACivObserverPawn::ACivObserverPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    BaseTurnRate = 55.f;
    BaseLookUpRate = 55.f;
}

void ACivObserverPawn::BeginPlay()
{
    Super::BeginPlay();
    FocusSettlement();
}

void ACivObserverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (!PlayerInputComponent) return;

    PlayerInputComponent->BindKey(EKeys::Home, IE_Pressed, this, &ACivObserverPawn::FocusSettlement);
    PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &ACivObserverPawn::FocusSettlement);
    PlayerInputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &ACivObserverPawn::CycleVillager);
    PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &ACivObserverPawn::FocusNearestVillager);
}

void ACivObserverPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UWorld* World = GetWorld();
    if (!World) return;

    ACivRuntimeEnvironment* Env = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(World, ACivRuntimeEnvironment::StaticClass()));
    if (!Env) return;

    FVector Location = GetActorLocation();
    const float GroundZ = Env->GetTerrainZAtWorldXY(Location.X, Location.Y);
    const float MinZ = GroundZ + MinHeightAboveTerrain;
    const float MaxZ = GroundZ + MaxHeightAboveTerrain;
    const float ClampedZ = FMath::Clamp(Location.Z, MinZ, MaxZ);
    if (!FMath::IsNearlyEqual(Location.Z, ClampedZ, 1.f))
    {
        Location.Z = ClampedZ;
        SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
    }
}

void ACivObserverPawn::FocusLocation(const FVector& Target, float Distance, float Height)
{
    const FVector ViewOffset(-Distance * 0.72f, -Distance, Height);
    const FVector NewLocation = Target + ViewOffset;
    SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
    SetActorRotation((Target + FVector(0.f, 0.f, 130.f) - NewLocation).Rotation());
}

void ACivObserverPawn::FocusSettlement()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(World, ACivWorldDirector::StaticClass()));
    const FVector Target = Director ? Director->GetActorLocation() : FVector::ZeroVector;
    FocusLocation(Target, 1150.f, 520.f);
}

void ACivObserverPawn::CycleVillager()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(World, ACivWorldDirector::StaticClass()));
    if (!Director || Director->Agents.Num() == 0) return;

    for (ACivAgentCharacter* Agent : Director->Agents)
    {
        if (Agent) Agent->SetObserverSelected(false);
    }

    for (int32 Attempt = 0; Attempt < Director->Agents.Num(); ++Attempt)
    {
        VillagerIndex = (VillagerIndex + 1) % Director->Agents.Num();
        ACivAgentCharacter* Agent = Director->Agents[VillagerIndex];
        if (!Agent || Agent->Health <= 0.f) continue;

        Agent->SetObserverSelected(true);
        FocusLocation(Agent->GetActorLocation(), 430.f, 240.f);
        break;
    }
}

void ACivObserverPawn::FocusNearestVillager()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(World, ACivWorldDirector::StaticClass()));
    if (!Director) return;

    ACivAgentCharacter* Best = nullptr;
    float BestDistanceSq = TNumericLimits<float>::Max();
    for (ACivAgentCharacter* Agent : Director->Agents)
    {
        if (!Agent || Agent->Health <= 0.f) continue;
        const float DistanceSq = FVector::DistSquared(GetActorLocation(), Agent->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            Best = Agent;
        }
    }

    if (!Best) return;
    for (ACivAgentCharacter* Agent : Director->Agents)
    {
        if (Agent) Agent->SetObserverSelected(Agent == Best);
    }
    FocusLocation(Best->GetActorLocation(), 430.f, 240.f);
}
