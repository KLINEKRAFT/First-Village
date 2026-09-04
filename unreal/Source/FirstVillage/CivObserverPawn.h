#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "CivObserverPawn.generated.h"

UCLASS()
class FIRSTVILLAGE_API ACivObserverPawn : public ADefaultPawn
{
    GENERATED_BODY()

public:
    ACivObserverPawn();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Observer")
    float MinHeightAboveTerrain = 160.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Observer")
    float MaxHeightAboveTerrain = 1350.f;

    UFUNCTION(BlueprintCallable, Category="Observer")
    void FocusSettlement();

    UFUNCTION(BlueprintCallable, Category="Observer")
    void CycleVillager();

    UFUNCTION(BlueprintCallable, Category="Observer")
    void FocusNearestVillager();

private:
    int32 VillagerIndex = INDEX_NONE;

    void FocusLocation(const FVector& Target, float Distance, float Height);
};
