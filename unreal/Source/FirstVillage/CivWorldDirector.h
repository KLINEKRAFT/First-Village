#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivWorldDirector.generated.h"

class ACivAgentCharacter;
class ACivResourceNode;
class ACivProceduralBuilding;

UCLASS()
class FIRSTVILLAGE_API ACivWorldDirector : public AActor
{
    GENERATED_BODY()

public:
    ACivWorldDirector();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simulation")
    int32 InitialPopulation = 12;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simulation")
    float SpawnRadius = 700.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simulation")
    bool bGenerateDemoVillage = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Simulation")
    TArray<TObjectPtr<ACivAgentCharacter>> Agents;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Simulation")
    TArray<TObjectPtr<ACivResourceNode>> Resources;

protected:
    virtual void BeginPlay() override;

private:
    void SpawnInitialPopulation();
    void SpawnResourceRing();
    void SpawnDemoStructures();
};
