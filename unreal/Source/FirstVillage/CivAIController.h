#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CivAIController.generated.h"

UCLASS()
class FIRSTVILLAGE_API ACivAIController : public AAIController
{
    GENERATED_BODY()

public:
    ACivAIController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    float DecisionTimer = 0.f;
    void ChooseLocalMovementGoal();
};
