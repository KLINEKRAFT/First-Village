#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivRiverActor.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class FIRSTVILLAGE_API ACivRiverActor : public AActor
{
    GENERATED_BODY()

public:
    ACivRiverActor();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Water;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> BankStones;

    void BuildRiver();
};
