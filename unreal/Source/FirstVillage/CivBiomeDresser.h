#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivBiomeDresser.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class ACivRuntimeEnvironment;

UCLASS()
class FIRSTVILLAGE_API ACivBiomeDresser : public AActor
{
    GENERATED_BODY()

public:
    ACivBiomeDresser();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    int32 TreeCount = 180;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    int32 RockCount = 90;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    float Radius = 5200.f;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TreeTrunks;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TreeCrowns;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Rocks;

    void ScatterInstances();
};
