#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CivAgentMindComponent.generated.h"

USTRUCT(BlueprintType)
struct FCivMemory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Text;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Source = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Confidence = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 DayCreated = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bFirsthand = true;
};

USTRUCT(BlueprintType)
struct FCivDecision
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Action = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 TargetAgentId = INDEX_NONE;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Resource = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Utterance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Reason;
};

UCLASS(ClassGroup=(FirstVillage), meta=(BlueprintSpawnableComponent))
class FIRSTVILLAGE_API UCivAgentMindComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCivAgentMindComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity") int32 AgentId = INDEX_NONE;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity") FString DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity") FName Role;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity") FName Trait;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity") FString PrivateGoal;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mind") TArray<FCivMemory> Memories;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mind") TMap<int32, float> Trust;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mind") TSet<FName> KnownFacts;

    UFUNCTION(BlueprintCallable, Category="Mind") void AddMemory(const FCivMemory& Memory);
    UFUNCTION(BlueprintCallable, Category="Mind") void LearnFact(FName FactId);
    UFUNCTION(BlueprintCallable, Category="Mind") void AdjustTrust(int32 OtherAgentId, float Delta);

    // Produces a JSON-compatible private observation. The world director should fill in
    // visible actors/resources before sending the payload to an external agent service.
    UFUNCTION(BlueprintCallable, Category="Mind") FString BuildPrivateObservationJson() const;
};