#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CivAgentApiSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCivAgentDecision, int32, AgentId, bool, bSuccess, const FString&, ResponseJson);

UCLASS()
class FIRSTVILLAGE_API UCivAgentApiSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent API")
    FString DecisionEndpoint = TEXT("http://127.0.0.1:8787/api/decide");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent API")
    float OfflineRetrySeconds = 10.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent API")
    bool bBackendReachable = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent API")
    int32 ConsecutiveFailures = 0;

    UPROPERTY(BlueprintAssignable, Category="Agent API")
    FOnCivAgentDecision OnDecisionReceived;

    UFUNCTION(BlueprintCallable, Category="Agent API")
    void RequestDecision(int32 AgentId, const FString& ObservationJson);

    UFUNCTION(BlueprintPure, Category="Agent API")
    FString GetStatusText() const;

private:
    bool bConnectionStateKnown = false;
    bool bProbeInFlight = false;
    double NextRetryWorldTime = 0.0;
};
