#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivTrailNetwork.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class ACivRuntimeEnvironment;

UCLASS()
class FIRSTVILLAGE_API ACivTrailNetwork : public AActor
{
    GENERATED_BODY()

public:
    ACivTrailNetwork();

    UFUNCTION(BlueprintCallable, Category="Trails")
    void AddTrail(const FVector& Start, const FVector& End, ACivRuntimeEnvironment* Environment, float Width = 140.f);

private:
    UPROPERTY() TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Segments;
};
