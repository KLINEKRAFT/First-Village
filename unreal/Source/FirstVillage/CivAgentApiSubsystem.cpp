#include "CivAgentApiSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

FString UCivAgentApiSubsystem::GetStatusText() const
{
    if (!bConnectionStateKnown) return TEXT("Brain: checking local server");
    if (bBackendReachable) return TEXT("Brain: connected");
    return TEXT("Brain: local fallback (server offline)");
}

void UCivAgentApiSubsystem::RequestDecision(int32 AgentId, const FString& ObservationJson)
{
    UWorld* World = GetWorld();
    const double Now = World ? World->GetTimeSeconds() : 0.0;

    if (!bConnectionStateKnown)
    {
        if (bProbeInFlight)
        {
            OnDecisionReceived.Broadcast(AgentId, false, FString());
            return;
        }
        bProbeInFlight = true;
    }
    else if (!bBackendReachable)
    {
        if (Now < NextRetryWorldTime || bProbeInFlight)
        {
            OnDecisionReceived.Broadcast(AgentId, false, FString());
            return;
        }
        bProbeInFlight = true;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(DecisionEndpoint);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(ObservationJson);
    Request->SetTimeout(8.f);

    Request->OnProcessRequestComplete().BindLambda([this, AgentId](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bConnectedSuccessfully)
    {
        const bool bOk = bConnectedSuccessfully && Resp.IsValid() && EHttpResponseCodes::IsOk(Resp->GetResponseCode());
        const double NowComplete = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

        bProbeInFlight = false;
        bConnectionStateKnown = true;
        bBackendReachable = bOk;

        if (bOk)
        {
            ConsecutiveFailures = 0;
            NextRetryWorldTime = 0.0;
        }
        else
        {
            ConsecutiveFailures++;
            NextRetryWorldTime = NowComplete + FMath::Max(OfflineRetrySeconds, 2.f);
        }

        OnDecisionReceived.Broadcast(AgentId, bOk, Resp.IsValid() ? Resp->GetContentAsString() : FString());
    });

    if (!Request->ProcessRequest())
    {
        bProbeInFlight = false;
        bConnectionStateKnown = true;
        bBackendReachable = false;
        ConsecutiveFailures++;
        NextRetryWorldTime = Now + FMath::Max(OfflineRetrySeconds, 2.f);
        OnDecisionReceived.Broadcast(AgentId, false, FString());
    }
}
