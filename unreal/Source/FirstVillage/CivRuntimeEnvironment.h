#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivRuntimeEnvironment.generated.h"

class UProceduralMeshComponent;
class UDirectionalLightComponent;
class USkyLightComponent;
class UExponentialHeightFogComponent;

UCLASS()
class FIRSTVILLAGE_API ACivRuntimeEnvironment : public AActor
{
    GENERATED_BODY()

public:
    ACivRuntimeEnvironment();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Environment")
    TObjectPtr<UProceduralMeshComponent> Terrain;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Environment")
    TObjectPtr<UDirectionalLightComponent> Sun;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Environment")
    TObjectPtr<USkyLightComponent> SkyLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Environment")
    TObjectPtr<UExponentialHeightFogComponent> Fog;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Environment|Terrain")
    int32 GridResolution = 64;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Environment|Terrain")
    float CellSize = 180.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Environment|Terrain")
    float HeightScale = 620.f;

    UFUNCTION(BlueprintCallable, Category="Environment")
    void GenerateValley();

protected:
    virtual void OnConstruction(const FTransform& Transform) override;

private:
    float SampleHeight(float X, float Y) const;
};
