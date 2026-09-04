#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivResourceNode.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

UENUM(BlueprintType)
enum class ECivResourceType : uint8
{
    Water,
    Food,
    Wood,
    Stone,
    Clay,
    Game
};

UCLASS()
class FIRSTVILLAGE_API ACivResourceNode : public AActor
{
    GENERATED_BODY()

public:
    ACivResourceNode();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
    ECivResourceType ResourceType = ECivResourceType::Wood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
    float Quantity = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
    bool bRenewable = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resource")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resource")
    TObjectPtr<UTextRenderComponent> Label;

    UFUNCTION(BlueprintCallable, Category="Resource")
    float Harvest(float RequestedAmount);

    UFUNCTION(BlueprintCallable, Category="Resource")
    void RefreshVisual();

private:
    FString ResourceLabel() const;
    FLinearColor ResourceColor() const;
};
