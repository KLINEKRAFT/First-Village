#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CivResourceNode.h"
#include "CivAIController.generated.h"

class ACivAgentCharacter;

UCLASS()
class FIRSTVILLAGE_API ACivAIController : public AAIController
{
    GENERATED_BODY()

public:
    ACivAIController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent Brain")
    bool bUseRemoteAgent = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent Brain")
    float DecisionIntervalMin = 12.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent Brain")
    float DecisionIntervalMax = 24.f;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
    float DecisionTimer = 0.f;
    bool bDecisionInFlight = false;
    FName ActiveAction = NAME_None;
    float PendingHarvestAmount = 0.f;
    ECivResourceType PendingHarvestType = ECivResourceType::Food;

    UPROPERTY()
    TObjectPtr<ACivResourceNode> ActiveResource;

    UPROPERTY()
    TObjectPtr<ACivAgentCharacter> ActiveOtherAgent;

    void ChooseLocalMovementGoal();
    void RequestRemoteDecision();
    FString BuildObservationJson() const;
    void ExecuteDecisionJson(const FString& ResponseJson);
    void ExecuteBuildDecision(const TSharedPtr<class FJsonObject>& DecisionObject);
    ACivResourceNode* FindNearestResource(FName ResourceName) const;
    void ReturnHarvestToSettlement();

    UFUNCTION()
    void HandleRemoteDecision(int32 AgentId, bool bSuccess, const FString& ResponseJson);
};
