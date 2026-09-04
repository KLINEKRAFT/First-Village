#include "CivAIController.h"
#include "CivAgentApiSubsystem.h"
#include "CivAgentCharacter.h"
#include "CivAgentMindComponent.h"
#include "CivBuildingValidator.h"
#include "CivProceduralBuilding.h"
#include "CivRuntimeEnvironment.h"
#include "CivWorldDirector.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
FString ResourceTypeToString(ECivResourceType Type)
{
    switch (Type)
    {
        case ECivResourceType::Water: return TEXT("water");
        case ECivResourceType::Food: return TEXT("food");
        case ECivResourceType::Wood: return TEXT("wood");
        case ECivResourceType::Stone: return TEXT("stone");
        case ECivResourceType::Clay: return TEXT("clay");
        case ECivResourceType::Game: return TEXT("game");
    }
    return TEXT("unknown");
}
}

ACivAIController::ACivAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACivAIController::BeginPlay()
{
    Super::BeginPlay();
    DecisionTimer = FMath::FRandRange(1.0f, 4.0f);

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (UCivAgentApiSubsystem* Api = GameInstance->GetSubsystem<UCivAgentApiSubsystem>())
            {
                Api->OnDecisionReceived.AddDynamic(this, &ACivAIController::HandleRemoteDecision);
            }
        }
    }
}

void ACivAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            if (UCivAgentApiSubsystem* Api = GameInstance->GetSubsystem<UCivAgentApiSubsystem>())
            {
                Api->OnDecisionReceived.RemoveDynamic(this, &ACivAIController::HandleRemoteDecision);
            }
        }
    }

    Super::EndPlay(EndPlayReason);
}

void ACivAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    if (!Agent || Agent->Health <= 0.f)
    {
        return;
    }

    if (!ActiveAction.IsNone() || bDecisionInFlight)
    {
        return;
    }

    DecisionTimer -= DeltaSeconds;
    if (DecisionTimer > 0.f)
    {
        return;
    }

    if (bUseRemoteAgent)
    {
        RequestRemoteDecision();
    }
    else
    {
        ChooseLocalMovementGoal();
        DecisionTimer = FMath::FRandRange(DecisionIntervalMin, DecisionIntervalMax);
    }
}

void ACivAIController::RequestRemoteDecision()
{
    ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    if (!Agent || !Agent->Mind)
    {
        ChooseLocalMovementGoal();
        DecisionTimer = 4.f;
        return;
    }

    UWorld* World = GetWorld();
    UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
    UCivAgentApiSubsystem* Api = GameInstance ? GameInstance->GetSubsystem<UCivAgentApiSubsystem>() : nullptr;
    if (!Api)
    {
        ChooseLocalMovementGoal();
        DecisionTimer = 4.f;
        return;
    }

    bDecisionInFlight = true;
    DecisionTimer = FMath::FRandRange(DecisionIntervalMin, DecisionIntervalMax);
    Api->RequestDecision(Agent->Mind->AgentId, BuildObservationJson());
}

FString ACivAIController::BuildObservationJson() const
{
    const ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    TSharedPtr<FJsonObject> Root;

    if (Agent && Agent->Mind)
    {
        const FString BaseJson = Agent->Mind->BuildPrivateObservationJson();
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BaseJson);
        FJsonSerializer::Deserialize(Reader, Root);
    }
    if (!Root.IsValid())
    {
        Root = MakeShared<FJsonObject>();
    }

    if (Agent)
    {
        TSharedRef<FJsonObject> Needs = MakeShared<FJsonObject>();
        Needs->SetNumberField(TEXT("health"), Agent->Health);
        Needs->SetNumberField(TEXT("hunger"), Agent->Hunger);
        Needs->SetNumberField(TEXT("thirst"), Agent->Thirst);
        Needs->SetNumberField(TEXT("fatigue"), Agent->Fatigue);
        Needs->SetNumberField(TEXT("morale"), Agent->Morale);
        Root->SetObjectField(TEXT("needs"), Needs);
    }

    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));
    if (Director)
    {
        TSharedRef<FJsonObject> Settlement = MakeShared<FJsonObject>();
        Settlement->SetNumberField(TEXT("food"), Director->FoodStore);
        Settlement->SetNumberField(TEXT("water"), Director->WaterStore);
        Settlement->SetNumberField(TEXT("wood"), Director->WoodStore);
        Settlement->SetNumberField(TEXT("stone"), Director->StoneStore);
        Settlement->SetNumberField(TEXT("clay"), Director->ClayStore);
        Settlement->SetNumberField(TEXT("thatch"), Director->ThatchStore);
        Settlement->SetNumberField(TEXT("population"), Director->Agents.Num());
        Root->SetObjectField(TEXT("settlement"), Settlement);

        TArray<TSharedPtr<FJsonValue>> VisibleResources;
        if (Agent)
        {
            for (ACivResourceNode* Node : Director->Resources)
            {
                if (!Node || Node->Quantity <= 0.f) continue;
                const float Distance = FVector::Dist2D(Agent->GetActorLocation(), Node->GetActorLocation());
                if (Distance > 2600.f) continue;

                TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
                Item->SetStringField(TEXT("type"), ResourceTypeToString(Node->ResourceType));
                Item->SetNumberField(TEXT("distance"), Distance);
                Item->SetNumberField(TEXT("estimated_quantity"), Node->Quantity);
                VisibleResources.Add(MakeShared<FJsonValueObject>(Item));
            }
        }
        Root->SetArrayField(TEXT("visible_resources"), VisibleResources);

        TArray<TSharedPtr<FJsonValue>> NearbyAgents;
        if (Agent)
        {
            for (ACivAgentCharacter* Other : Director->Agents)
            {
                if (!Other || Other == Agent || !Other->Mind || Other->Health <= 0.f) continue;
                const float Distance = FVector::Dist2D(Agent->GetActorLocation(), Other->GetActorLocation());
                if (Distance > 1600.f) continue;

                TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
                Item->SetNumberField(TEXT("id"), Other->Mind->AgentId);
                Item->SetStringField(TEXT("name"), Other->Mind->DisplayName);
                Item->SetStringField(TEXT("role"), Other->Mind->Role.ToString());
                Item->SetNumberField(TEXT("distance"), Distance);
                NearbyAgents.Add(MakeShared<FJsonValueObject>(Item));
            }
        }
        Root->SetArrayField(TEXT("nearby_agents"), NearbyAgents);
    }

    const TCHAR* Actions[] = {
        TEXT("explore"), TEXT("gather"), TEXT("hunt"), TEXT("eat"), TEXT("drink"), TEXT("sleep"), TEXT("rest"),
        TEXT("talk"), TEXT("teach"), TEXT("heal"), TEXT("experiment"), TEXT("craft"), TEXT("build"), TEXT("vote")
    };
    TArray<TSharedPtr<FJsonValue>> AllowedActions;
    for (const TCHAR* Action : Actions)
    {
        AllowedActions.Add(MakeShared<FJsonValueString>(Action));
    }
    Root->SetArrayField(TEXT("allowed_actions"), AllowedActions);

    FString Output;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
    return Output;
}

void ACivAIController::HandleRemoteDecision(int32 AgentId, bool bSuccess, const FString& ResponseJson)
{
    ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    if (!Agent || !Agent->Mind || Agent->Mind->AgentId != AgentId)
    {
        return;
    }

    bDecisionInFlight = false;
    DecisionTimer = FMath::FRandRange(DecisionIntervalMin, DecisionIntervalMax);

    if (!bSuccess || ResponseJson.IsEmpty())
    {
        ChooseLocalMovementGoal();
        return;
    }

    ExecuteDecisionJson(ResponseJson);
}

void ACivAIController::ExecuteDecisionJson(const FString& ResponseJson)
{
    TSharedPtr<FJsonObject> Decision;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseJson);
    if (!FJsonSerializer::Deserialize(Reader, Decision) || !Decision.IsValid())
    {
        ChooseLocalMovementGoal();
        return;
    }

    FString Action;
    if (!Decision->TryGetStringField(TEXT("action"), Action))
    {
        ChooseLocalMovementGoal();
        return;
    }

    ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    if (!Agent) return;

    ActiveAction = FName(*Action);

    if (Action == TEXT("explore"))
    {
        ActiveAction = NAME_None;
        ChooseLocalMovementGoal();
        return;
    }

    if (Action == TEXT("rest") || Action == TEXT("sleep"))
    {
        StopMovement();
        Agent->ApplyNeedDelta(0.f, 0.f, Action == TEXT("sleep") ? -55.f : -25.f, 2.f);
        ActiveAction = NAME_None;
        DecisionTimer = Action == TEXT("sleep") ? 28.f : 12.f;
        return;
    }

    if (Action == TEXT("build"))
    {
        ExecuteBuildDecision(Decision);
        ActiveAction = NAME_None;
        return;
    }

    if (Action == TEXT("gather") || Action == TEXT("hunt") || Action == TEXT("drink") || Action == TEXT("eat"))
    {
        FString Resource;
        if (Action == TEXT("hunt")) Resource = TEXT("game");
        else if (Action == TEXT("drink")) Resource = TEXT("water");
        else if (Action == TEXT("eat")) Resource = TEXT("food");
        else Decision->TryGetStringField(TEXT("resource"), Resource);

        ActiveResource = FindNearestResource(FName(*Resource));
        if (ActiveResource)
        {
            MoveToActor(ActiveResource, 110.f, true, true, true, nullptr, true);
            return;
        }

        ActiveAction = NAME_None;
        ChooseLocalMovementGoal();
        return;
    }

    if (Action == TEXT("talk") || Action == TEXT("teach") || Action == TEXT("heal"))
    {
        double TargetNumber = INDEX_NONE;
        Decision->TryGetNumberField(TEXT("target_id"), TargetNumber);
        ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));
        ActiveOtherAgent = Director ? Director->FindAgentById(static_cast<int32>(TargetNumber)) : nullptr;
        if (ActiveOtherAgent)
        {
            MoveToActor(ActiveOtherAgent, 135.f, true, true, true, nullptr, true);
            return;
        }

        ActiveAction = NAME_None;
        ChooseLocalMovementGoal();
        return;
    }

    if (Action == TEXT("experiment") || Action == TEXT("craft") || Action == TEXT("vote"))
    {
        StopMovement();
        if (Agent->Mind)
        {
            FCivMemory Memory;
            Memory.Text = FString::Printf(TEXT("I chose to %s based on what I knew at the time."), *Action);
            Memory.Source = TEXT("self");
            Memory.Confidence = 1.f;
            Memory.bFirsthand = true;
            Agent->Mind->AddMemory(Memory);
        }
        Agent->Morale = FMath::Clamp(Agent->Morale + 1.f, 0.f, 100.f);
        ActiveAction = NAME_None;
        DecisionTimer = 10.f;
        return;
    }

    ActiveAction = NAME_None;
    ChooseLocalMovementGoal();
}

ACivResourceNode* ACivAIController::FindNearestResource(FName ResourceName) const
{
    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));
    const APawn* Pawn = GetPawn();
    if (!Director || !Pawn) return nullptr;

    ECivResourceType Desired = ECivResourceType::Food;
    if (ResourceName == TEXT("water")) Desired = ECivResourceType::Water;
    else if (ResourceName == TEXT("wood")) Desired = ECivResourceType::Wood;
    else if (ResourceName == TEXT("stone")) Desired = ECivResourceType::Stone;
    else if (ResourceName == TEXT("clay")) Desired = ECivResourceType::Clay;
    else if (ResourceName == TEXT("game")) Desired = ECivResourceType::Game;
    else Desired = ECivResourceType::Food;

    ACivResourceNode* Best = nullptr;
    float BestDistanceSq = TNumericLimits<float>::Max();
    for (ACivResourceNode* Node : Director->Resources)
    {
        if (!Node || Node->Quantity <= 0.f || Node->ResourceType != Desired) continue;
        const float DistanceSq = FVector::DistSquared2D(Pawn->GetActorLocation(), Node->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            Best = Node;
        }
    }
    return Best;
}

void ACivAIController::ReturnHarvestToSettlement()
{
    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));
    if (!Director)
    {
        ActiveAction = NAME_None;
        PendingHarvestAmount = 0.f;
        return;
    }

    ActiveAction = TEXT("return_harvest");
    MoveToLocation(Director->GetActorLocation(), 180.f, true, true, true, false, nullptr, true);
}

void ACivAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));
    if (!Agent)
    {
        ActiveAction = NAME_None;
        return;
    }

    if (!Result.IsSuccess())
    {
        ActiveAction = NAME_None;
        ActiveResource = nullptr;
        ActiveOtherAgent = nullptr;
        PendingHarvestAmount = 0.f;
        DecisionTimer = 3.f;
        return;
    }

    if (ActiveAction == TEXT("gather") || ActiveAction == TEXT("hunt"))
    {
        if (ActiveResource)
        {
            PendingHarvestType = ActiveResource->ResourceType;
            PendingHarvestAmount = ActiveResource->Harvest(ActiveAction == TEXT("hunt") ? 6.f : 4.f);
            if (Agent->Mind)
            {
                Agent->Mind->LearnFact(FName(*FString::Printf(TEXT("resource_%s"), *ResourceTypeToString(PendingHarvestType))));
            }
        }
        ActiveResource = nullptr;
        if (PendingHarvestAmount > 0.f)
        {
            ReturnHarvestToSettlement();
            return;
        }
    }
    else if (ActiveAction == TEXT("return_harvest"))
    {
        if (Director && PendingHarvestAmount > 0.f)
        {
            Director->AddHarvest(PendingHarvestType, PendingHarvestAmount);
            if (Agent->Mind)
            {
                FCivMemory Memory;
                Memory.Text = FString::Printf(TEXT("I brought %.1f %s back to our common stores."), PendingHarvestAmount, *ResourceTypeToString(PendingHarvestType));
                Memory.Source = TEXT("self");
                Memory.Confidence = 1.f;
                Memory.bFirsthand = true;
                Agent->Mind->AddMemory(Memory);
            }
        }
        PendingHarvestAmount = 0.f;
    }
    else if (ActiveAction == TEXT("drink"))
    {
        if (ActiveResource)
        {
            ActiveResource->Harvest(1.f);
            Agent->ApplyNeedDelta(0.f, -65.f, 0.f, 1.f);
            if (Agent->Mind) Agent->Mind->LearnFact(TEXT("resource_water"));
        }
    }
    else if (ActiveAction == TEXT("eat"))
    {
        if (ActiveResource)
        {
            ActiveResource->Harvest(1.f);
            Agent->ApplyNeedDelta(-60.f, 0.f, 0.f, 2.f);
            if (Agent->Mind) Agent->Mind->LearnFact(TEXT("resource_food"));
        }
    }
    else if (ActiveAction == TEXT("talk") || ActiveAction == TEXT("teach") || ActiveAction == TEXT("heal"))
    {
        if (ActiveOtherAgent && ActiveOtherAgent->Mind && Agent->Mind)
        {
            Agent->Mind->AdjustTrust(ActiveOtherAgent->Mind->AgentId, 2.f);
            ActiveOtherAgent->Mind->AdjustTrust(Agent->Mind->AgentId, 2.f);

            if (ActiveAction == TEXT("heal"))
            {
                ActiveOtherAgent->ApplyHealthDelta(14.f);
                ActiveOtherAgent->Morale = FMath::Clamp(ActiveOtherAgent->Morale + 3.f, 0.f, 100.f);
            }
            else if (ActiveAction == TEXT("teach"))
            {
                for (const FName Fact : Agent->Mind->KnownFacts)
                {
                    if (!ActiveOtherAgent->Mind->KnownFacts.Contains(Fact))
                    {
                        ActiveOtherAgent->Mind->LearnFact(Fact);
                        break;
                    }
                }
            }

            FCivMemory Memory;
            Memory.Text = FString::Printf(TEXT("I %s with %s."), *ActiveAction.ToString(), *ActiveOtherAgent->Mind->DisplayName);
            Memory.Source = TEXT("self");
            Memory.Confidence = 1.f;
            Memory.bFirsthand = true;
            Agent->Mind->AddMemory(Memory);
        }
    }

    ActiveAction = NAME_None;
    ActiveResource = nullptr;
    ActiveOtherAgent = nullptr;
    DecisionTimer = FMath::FRandRange(DecisionIntervalMin, DecisionIntervalMax);
}

void ACivAIController::ExecuteBuildDecision(const TSharedPtr<FJsonObject>& DecisionObject)
{
    ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));
    if (!Agent || !Agent->Mind || !Director || !DecisionObject.IsValid()) return;

    const TSharedPtr<FJsonObject>* BlueprintJsonPtr = nullptr;
    if (!DecisionObject->TryGetObjectField(TEXT("blueprint"), BlueprintJsonPtr) || !BlueprintJsonPtr || !BlueprintJsonPtr->IsValid())
    {
        return;
    }
    const TSharedPtr<FJsonObject>& BlueprintJson = *BlueprintJsonPtr;

    FCivBuildingBlueprint Blueprint;
    BlueprintJson->TryGetStringField(TEXT("name"), Blueprint.Name);
    BlueprintJson->TryGetStringField(TEXT("purpose"), Blueprint.Purpose);

    const TSharedPtr<FJsonObject>* FootprintPtr = nullptr;
    if (BlueprintJson->TryGetObjectField(TEXT("footprint"), FootprintPtr) && FootprintPtr && FootprintPtr->IsValid())
    {
        double W = 8.0;
        double H = 8.0;
        (*FootprintPtr)->TryGetNumberField(TEXT("w"), W);
        (*FootprintPtr)->TryGetNumberField(TEXT("h"), H);
        Blueprint.Footprint = FVector2D(static_cast<float>(W), static_cast<float>(H));
    }

    TMap<FName, float> Costs;
    const TSharedPtr<FJsonObject>* MaterialsPtr = nullptr;
    if (BlueprintJson->TryGetObjectField(TEXT("materials"), MaterialsPtr) && MaterialsPtr && MaterialsPtr->IsValid())
    {
        const FName MaterialNames[] = { TEXT("wood"), TEXT("stone"), TEXT("thatch"), TEXT("clay") };
        for (const FName MaterialName : MaterialNames)
        {
            double Amount = 0.0;
            (*MaterialsPtr)->TryGetNumberField(MaterialName.ToString(), Amount);
            if (Amount > 0.0)
            {
                FCivMaterialCost Cost;
                Cost.Material = MaterialName;
                Cost.Amount = static_cast<float>(Amount);
                Blueprint.Materials.Add(Cost);
                Costs.Add(MaterialName, Cost.Amount);
            }
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* PrimitiveValues = nullptr;
    if (BlueprintJson->TryGetArrayField(TEXT("primitives"), PrimitiveValues) && PrimitiveValues)
    {
        for (const TSharedPtr<FJsonValue>& Value : *PrimitiveValues)
        {
            FString Primitive;
            if (Value.IsValid() && Value->TryGetString(Primitive))
            {
                Blueprint.Primitives.Add(FName(*Primitive));
            }
        }
    }

    TMap<FName, float> AvailableMaterials;
    Director->GetMaterialStores(AvailableMaterials);
    UCivBuildingValidator* Validator = NewObject<UCivBuildingValidator>(this);
    const FCivValidationResult Validation = Validator->ValidateBlueprint(Blueprint, AvailableMaterials, Agent->Mind->KnownFacts);
    if (!Validation.bAccepted || !Director->ConsumeMaterials(Costs))
    {
        FCivMemory Memory;
        Memory.Text = FString::Printf(TEXT("My plan for %s could not be built: %s"), *Blueprint.Name, *Validation.Reason);
        Memory.Source = TEXT("self");
        Memory.Confidence = 1.f;
        Memory.bFirsthand = true;
        Agent->Mind->AddMemory(Memory);
        return;
    }

    const float Angle = FMath::FRandRange(0.f, 2.f * PI);
    const float Radius = FMath::FRandRange(650.f, 1200.f);
    FVector Site = Director->GetActorLocation() + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);
    if (ACivRuntimeEnvironment* Environment = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivRuntimeEnvironment::StaticClass())))
    {
        Site.Z = Environment->GetTerrainZAtWorldXY(Site.X, Site.Y) + 4.f;
    }

    ACivProceduralBuilding* Building = GetWorld()->SpawnActor<ACivProceduralBuilding>(Site, FRotator::ZeroRotator);
    if (Building)
    {
        Building->Blueprint = Blueprint;
        Building->RebuildFromBlueprint();

        FCivMemory Memory;
        Memory.Text = FString::Printf(TEXT("I helped create the %s for %s."), *Blueprint.Name, *Blueprint.Purpose);
        Memory.Source = TEXT("self");
        Memory.Confidence = 1.f;
        Memory.bFirsthand = true;
        Agent->Mind->AddMemory(Memory);
    }
}

void ACivAIController::ChooseLocalMovementGoal()
{
    ACivAgentCharacter* Agent = Cast<ACivAgentCharacter>(GetPawn());
    if (!Agent) return;

    if (Agent->Thirst > 72.f)
    {
        ActiveAction = TEXT("drink");
        ActiveResource = FindNearestResource(TEXT("water"));
        if (ActiveResource)
        {
            MoveToActor(ActiveResource, 110.f, true, true, true, nullptr, true);
            return;
        }
        ActiveAction = NAME_None;
    }

    if (Agent->Hunger > 74.f)
    {
        ActiveAction = TEXT("eat");
        ActiveResource = FindNearestResource(TEXT("food"));
        if (ActiveResource)
        {
            MoveToActor(ActiveResource, 110.f, true, true, true, nullptr, true);
            return;
        }
        ActiveAction = NAME_None;
    }

    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));
    if (Director)
    {
        FName NeededResource = NAME_None;
        if (Director->WaterStore < 8.f) NeededResource = TEXT("water");
        else if (Director->FoodStore < 8.f) NeededResource = TEXT("food");
        else if (Director->WoodStore < 12.f) NeededResource = TEXT("wood");

        if (!NeededResource.IsNone())
        {
            ActiveResource = FindNearestResource(NeededResource);
            if (ActiveResource)
            {
                ActiveAction = TEXT("gather");
                MoveToActor(ActiveResource, 110.f, true, true, true, nullptr, true);
                return;
            }
        }
    }

    UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!Nav) return;

    FNavLocation Goal;
    if (Nav->GetRandomReachablePointInRadius(Agent->GetActorLocation(), 1800.f, Goal))
    {
        MoveToLocation(Goal.Location, 80.f, true, true, true, false, nullptr, true);
    }
}
