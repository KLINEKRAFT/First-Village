#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CivBuildingValidator.generated.h"

USTRUCT(BlueprintType)
struct FCivMaterialCost
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Material;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float Amount = 0.0f;
};

USTRUCT(BlueprintType)
struct FCivBuildingBlueprint
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Purpose;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D Footprint = FVector2D(20.0f, 20.0f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCivMaterialCost> Materials;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> Primitives;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> RequiredTechniques;
};

USTRUCT(BlueprintType)
struct FCivValidationResult
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bAccepted = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Reason;
};

UCLASS(BlueprintType)
class FIRSTVILLAGE_API UCivBuildingValidator : public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="First Village|Building")
    FCivValidationResult ValidateBlueprint(const FCivBuildingBlueprint& Blueprint,
        const TMap<FName, float>& AvailableMaterials,
        const TSet<FName>& KnownTechniques) const;

    UFUNCTION(BlueprintPure, Category="First Village|Building")
    static bool IsAllowedPrimitive(FName Primitive);
};