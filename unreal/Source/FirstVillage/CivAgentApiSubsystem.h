#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CivAgentApiSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCivAgentDecision, bool, bSuccess, const FString&, ResponseJson);

UCLASS()
class FIRSTVILLAGE_API UCivAgentApiSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent API")
    FString DecisionEndpoint = TEXT("http://127.0.0.1:8787/api/decide");

    UPROPERTY(BlueprintAssignable, Category="Agent API")
    FOnCivAgentDecision OnDecisionReceived;

    UFUNCTION(BlueprintCallable, Category="Agent API")
    void RequestDecision(const FString& ObservationJson);
};
