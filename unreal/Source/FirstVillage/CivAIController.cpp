#include "CivAIController.h"
#include "NavigationSystem.h"

ACivAIController::ACivAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACivAIController::BeginPlay()
{
    Super::BeginPlay();
    DecisionTimer = FMath::FRandRange(0.5f, 2.0f);
}

void ACivAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    DecisionTimer -= DeltaSeconds;
    if (DecisionTimer <= 0.f)
    {
        ChooseLocalMovementGoal();
        DecisionTimer = FMath::FRandRange(4.f, 9.f);
    }
}

void ACivAIController::ChooseLocalMovementGoal()
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!Nav)
    {
        return;
    }

    FNavLocation Goal;
    if (Nav->GetRandomReachablePointInRadius(ControlledPawn->GetActorLocation(), 1800.f, Goal))
    {
        MoveToLocation(Goal.Location, 80.f, true, true, true, false, nullptr, true);
    }
}
