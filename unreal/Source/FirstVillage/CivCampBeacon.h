#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivCampBeacon.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class UTextRenderComponent;

UCLASS()
class FIRSTVILLAGE_API ACivCampBeacon : public AActor
{
    GENERATED_BODY()

public:
    ACivCampBeacon();
    virtual void Tick(float DeltaSeconds) override;

private:
    UPROPERTY() TObjectPtr<USceneComponent> Root;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> FireCore;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> HearthStoneA;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> HearthStoneB;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> HearthStoneC;
    UPROPERTY() TObjectPtr<UPointLightComponent> FireLight;
    UPROPERTY() TObjectPtr<UTextRenderComponent> Label;
};
