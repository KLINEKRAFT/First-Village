#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CivAgentCharacter.generated.h"

class UCivAgentMindComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UMaterialInstanceDynamic;

UCLASS()
class FIRSTVILLAGE_API ACivAgentCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACivAgentCharacter();
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent")
    TObjectPtr<UCivAgentMindComponent> Mind;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent|Visual")
    TObjectPtr<UStaticMeshComponent> BodyVisual;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent|Visual")
    TObjectPtr<UStaticMeshComponent> HeadVisual;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent|Visual")
    TObjectPtr<UStaticMeshComponent> LeftLegVisual;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent|Visual")
    TObjectPtr<UStaticMeshComponent> RightLegVisual;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent|Visual")
    TObjectPtr<UStaticMeshComponent> LeftArmVisual;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent|Visual")
    TObjectPtr<UStaticMeshComponent> RightArmVisual;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Agent|Visual")
    TObjectPtr<UTextRenderComponent> Nameplate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent|Needs")
    float Health = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent|Needs")
    float Hunger = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent|Needs")
    float Thirst = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent|Needs")
    float Fatigue = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent|Needs")
    float Morale = 80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Agent|Needs")
    float NeedTimeScale = 1.f;

    UFUNCTION(BlueprintCallable, Category="Agent")
    void ApplyNeedDelta(float HungerDelta, float ThirstDelta, float FatigueDelta, float MoraleDelta);

    UFUNCTION(BlueprintCallable, Category="Agent")
    void ApplyHealthDelta(float Delta);

    UFUNCTION(BlueprintCallable, Category="Agent|Visual")
    void RefreshVisualIdentity();

    UFUNCTION(BlueprintCallable, Category="Agent|Visual")
    void SetObserverSelected(bool bSelected);

private:
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> RoleMaterial;

    bool bObserverSelected = false;
    FLinearColor RoleColor() const;
};
