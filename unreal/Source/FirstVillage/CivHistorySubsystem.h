#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CivHistorySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FCivHistoryEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Day = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Type = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Summary;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<int32> AgentIds;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector WorldLocation = FVector::ZeroVector;
};

UCLASS()
class FIRSTVILLAGE_API UCivHistorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="History")
    TArray<FCivHistoryEvent> Events;

    UFUNCTION(BlueprintCallable, Category="History")
    void RecordEvent(const FCivHistoryEvent& Event);

    UFUNCTION(BlueprintPure, Category="History")
    TArray<FCivHistoryEvent> GetRecentEvents(int32 MaxCount = 25) const;
};
