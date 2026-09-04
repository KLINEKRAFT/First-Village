#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivProceduralBuilding.generated.h"

USTRUCT(BlueprintType)
struct FCivBuildingBlueprint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name = TEXT("Hut");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Purpose = TEXT("Housing");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D Footprint = FVector2D(500.f, 400.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WallHeight = 280.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasRoof = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasHearth = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PostCount = 6;
};

UCLASS()
class FIRSTVILLAGE_API ACivProceduralBuilding : public AActor
{
    GENERATED_BODY()

public:
    ACivProceduralBuilding();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building")
    FCivBuildingBlueprint Blueprint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Building")
    TObjectPtr<USceneComponent> Root;

    UFUNCTION(BlueprintCallable, Category="Building")
    void RebuildFromBlueprint();

protected:
    virtual void OnConstruction(const FTransform& Transform) override;

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<UStaticMeshComponent>> GeneratedParts;

    UStaticMeshComponent* AddPrimitive(const FName& Name, const FVector& RelativeLocation, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);
    void ClearGeneratedParts();
};
