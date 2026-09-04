#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivBiomeDresser.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class FIRSTVILLAGE_API ACivBiomeDresser : public AActor
{
    GENERATED_BODY()

public:
    ACivBiomeDresser();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    int32 TreeCount = 280;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    int32 RockCount = 120;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    int32 ShrubCount = 220;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    int32 ReedCount = 260;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    int32 GrassCount = 850;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Biome")
    float Radius = 5200.f;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TreeTrunks;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> TreeCrowns;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Rocks;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Shrubs;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Reeds;
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Grass;

    void ScatterInstances();
};
