#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivBuildingValidator.h"
#include "CivProceduralBuilding.generated.h"

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
    bool HasPrimitive(FName Primitive) const;
};
