#include "CivAgentApiSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UCivAgentApiSubsystem::RequestDecision(int32 AgentId, const FString& ObservationJson)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(DecisionEndpoint);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(ObservationJson);

    Request->OnProcessRequestComplete().BindLambda([this, AgentId](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bConnectedSuccessfully)
    {
        const bool bOk = bConnectedSuccessfully && Resp.IsValid() && EHttpResponseCodes::IsOk(Resp->GetResponseCode());
        OnDecisionReceived.Broadcast(AgentId, bOk, Resp.IsValid() ? Resp->GetContentAsString() : FString());
    });

    Request->ProcessRequest();
}
