#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivResourceNode.h"
#include "CivWorldDirector.generated.h"

class ACivAgentCharacter;
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settlement|Stores")
    float FoodStore = 13.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settlement|Stores")
    float WaterStore = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settlement|Stores")
    float WoodStore = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settlement|Stores")
    float StoneStore = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settlement|Stores")
    float ClayStore = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settlement|Stores")
    float ThatchStore = 5.f;

    UFUNCTION(BlueprintCallable, Category="Settlement")
    void AddHarvest(ECivResourceType Type, float Amount);

    UFUNCTION(BlueprintPure, Category="Settlement")
    float GetStoreAmount(FName Resource) const;

    UFUNCTION(BlueprintCallable, Category="Settlement")
    bool ConsumeMaterials(const TMap<FName, float>& Costs);

    UFUNCTION(BlueprintPure, Category="Settlement")
    ACivAgentCharacter* FindAgentById(int32 AgentId) const;

    void GetMaterialStores(TMap<FName, float>& OutStores) const;

protected:
    virtual void BeginPlay() override;

private:
    void SpawnInitialPopulation();
    void SpawnResourceRing();
    void SpawnDemoStructures();
    FVector ProjectToGround(const FVector& DesiredLocation, float HeightOffset = 0.f) const;
};
