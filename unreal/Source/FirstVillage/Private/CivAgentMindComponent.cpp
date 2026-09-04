#include "CivAgentMindComponent.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

UCivAgentMindComponent::UCivAgentMindComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCivAgentMindComponent::AddMemory(const FCivMemory& Memory)
{
    Memories.Insert(Memory, 0);
    constexpr int32 MaxMemories = 64;
    if (Memories.Num() > MaxMemories)
    {
        Memories.SetNum(MaxMemories);
    }
}

void UCivAgentMindComponent::LearnFact(FName FactId)
{
    if (!FactId.IsNone())
    {
        KnownFacts.Add(FactId);
    }
}

void UCivAgentMindComponent::AdjustTrust(int32 OtherAgentId, float Delta)
{
    float& Value = Trust.FindOrAdd(OtherAgentId, 50.0f);
    Value = FMath::Clamp(Value + Delta, 0.0f, 100.0f);
}

FString UCivAgentMindComponent::BuildPrivateObservationJson() const
{
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetNumberField(TEXT("agent_id"), AgentId);

    TSharedRef<FJsonObject> Identity = MakeShared<FJsonObject>();
    Identity->SetStringField(TEXT("name"), DisplayName);
    Identity->SetStringField(TEXT("role"), Role.ToString());
    Identity->SetStringField(TEXT("trait"), Trait.ToString());
    Identity->SetStringField(TEXT("private_goal"), PrivateGoal);
    Root->SetObjectField(TEXT("identity"), Identity);

    TArray<TSharedPtr<FJsonValue>> Facts;
    for (const FName Fact : KnownFacts)
    {
        Facts.Add(MakeShared<FJsonValueString>(Fact.ToString()));
    }
    Root->SetArrayField(TEXT("known_facts"), Facts);

    TArray<TSharedPtr<FJsonValue>> MemoryJson;
    const int32 Count = FMath::Min(Memories.Num(), 12);
    for (int32 Index = 0; Index < Count; ++Index)
    {
        const FCivMemory& M = Memories[Index];
        TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("text"), M.Text);
        Item->SetStringField(TEXT("source"), M.Source.ToString());
        Item->SetNumberField(TEXT("confidence"), M.Confidence);
        Item->SetNumberField(TEXT("day"), M.DayCreated);
        Item->SetBoolField(TEXT("firsthand"), M.bFirsthand);
        MemoryJson.Add(MakeShared<FJsonValueObject>(Item));
    }
    Root->SetArrayField(TEXT("memories"), MemoryJson);

    FString Output;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    FJsonSerializer::Serialize(Root, Writer);
    return Output;
}