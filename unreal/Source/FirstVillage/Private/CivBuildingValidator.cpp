#include "CivBuildingValidator.h"

bool UCivBuildingValidator::IsAllowedPrimitive(FName Primitive)
{
    static const TSet<FName> Allowed = {
        TEXT("walls"), TEXT("roof"), TEXT("posts"), TEXT("hearth"),
        TEXT("storage_bins"), TEXT("drying_beams"), TEXT("fence"), TEXT("pit"),
        TEXT("platform"), TEXT("channel"), TEXT("doorway"), TEXT("work_surface")
    };
    return Allowed.Contains(Primitive);
}

FCivValidationResult UCivBuildingValidator::ValidateBlueprint(
    const FCivBuildingBlueprint& Blueprint,
    const TMap<FName, float>& AvailableMaterials,
    const TSet<FName>& KnownTechniques) const
{
    FCivValidationResult Result;

    if (Blueprint.Name.IsEmpty() || Blueprint.Purpose.IsEmpty())
    {
        Result.Reason = TEXT("Blueprint requires a name and purpose.");
        return Result;
    }

    if (Blueprint.Footprint.X < 8.0f || Blueprint.Footprint.X > 80.0f ||
        Blueprint.Footprint.Y < 8.0f || Blueprint.Footprint.Y > 80.0f)
    {
        Result.Reason = TEXT("Footprint is outside allowed bounds.");
        return Result;
    }

    for (const FName Primitive : Blueprint.Primitives)
    {
        if (!IsAllowedPrimitive(Primitive))
        {
            Result.Reason = FString::Printf(TEXT("Primitive '%s' is not supported."), *Primitive.ToString());
            return Result;
        }
    }

    for (const FName Technique : Blueprint.RequiredTechniques)
    {
        if (!KnownTechniques.Contains(Technique))
        {
            Result.Reason = FString::Printf(TEXT("Required technique '%s' is not known."), *Technique.ToString());
            return Result;
        }
    }

    for (const FCivMaterialCost& Cost : Blueprint.Materials)
    {
        if (Cost.Amount < 0.0f || Cost.Amount > 100.0f)
        {
            Result.Reason = TEXT("Material quantity is invalid.");
            return Result;
        }
        const float* Available = AvailableMaterials.Find(Cost.Material);
        if (!Available || *Available < Cost.Amount)
        {
            Result.Reason = FString::Printf(TEXT("Insufficient %s."), *Cost.Material.ToString());
            return Result;
        }
    }

    Result.bAccepted = true;
    Result.Reason = TEXT("Blueprint passes logical validation. Terrain/site validation must run before construction.");
    return Result;
}