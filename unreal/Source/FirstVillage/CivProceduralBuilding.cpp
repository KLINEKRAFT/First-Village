#include "CivProceduralBuilding.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/UObjectGlobals.h"

namespace
{
FLinearColor PartColor(const FName& Name)
{
    const FString S = Name.ToString();
    if (S.Contains(TEXT("Roof"))) return FLinearColor(0.38f, 0.23f, 0.07f, 1.f);
    if (S.Contains(TEXT("Wall"))) return FLinearColor(0.43f, 0.30f, 0.17f, 1.f);
    if (S.Contains(TEXT("Post")) || S.Contains(TEXT("Beam")) || S.Contains(TEXT("Lintel"))) return FLinearColor(0.20f, 0.10f, 0.035f, 1.f);
    if (S.Contains(TEXT("Hearth")) || S.Contains(TEXT("Pit"))) return FLinearColor(0.25f, 0.24f, 0.21f, 1.f);
    if (S.Contains(TEXT("Storage"))) return FLinearColor(0.33f, 0.19f, 0.07f, 1.f);
    if (S.Contains(TEXT("Fence"))) return FLinearColor(0.24f, 0.12f, 0.04f, 1.f);
    if (S.Contains(TEXT("Channel"))) return FLinearColor(0.18f, 0.22f, 0.16f, 1.f);
    return FLinearColor(0.31f, 0.19f, 0.08f, 1.f);
}
}

ACivProceduralBuilding::ACivProceduralBuilding()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Blueprint.Name = TEXT("Hut");
    Blueprint.Purpose = TEXT("Housing");
    Blueprint.Footprint = FVector2D(8.f, 8.f);
    Blueprint.Primitives = { TEXT("walls"), TEXT("roof"), TEXT("posts"), TEXT("doorway") };
}

void ACivProceduralBuilding::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    RebuildFromBlueprint();
}

bool ACivProceduralBuilding::HasPrimitive(FName Primitive) const
{
    return Blueprint.Primitives.Contains(Primitive);
}

void ACivProceduralBuilding::ClearGeneratedParts()
{
    for (UStaticMeshComponent* Part : GeneratedParts)
    {
        if (Part) Part->DestroyComponent();
    }
    GeneratedParts.Reset();
}

UStaticMeshComponent* ACivProceduralBuilding::AddPrimitive(const FName& Name, const FVector& RelativeLocation, const FVector& Scale, const FRotator& Rotation)
{
    const FName UniqueName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), Name);
    UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(this, UniqueName);
    Part->SetupAttachment(Root);
    Part->RegisterComponent();
    Part->SetRelativeLocation(RelativeLocation);
    Part->SetRelativeRotation(Rotation);
    Part->SetRelativeScale3D(Scale);
    Part->SetCollisionProfileName(TEXT("BlockAll"));

    if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"))) Part->SetStaticMesh(Cube);

    if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
    {
        if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Part))
        {
            MID->SetVectorParameterValue(TEXT("Color"), PartColor(Name));
            Part->SetMaterial(0, MID);
        }
    }

    GeneratedParts.Add(Part);
    return Part;
}

void ACivProceduralBuilding::RebuildFromBlueprint()
{
    ClearGeneratedParts();

    const float HalfX = FMath::Clamp(Blueprint.Footprint.X, 8.f, 80.f) * 50.f;
    const float HalfY = FMath::Clamp(Blueprint.Footprint.Y, 8.f, 80.f) * 50.f;
    const float WallT = 18.f;
    const float WallH = FMath::Clamp(220.f + FMath::Min(Blueprint.Footprint.X, Blueprint.Footprint.Y) * 8.f, 240.f, 420.f);

    if (HasPrimitive(TEXT("walls")))
    {
        AddPrimitive(TEXT("WallNorth"), FVector(0.f, HalfY, WallH * 0.5f), FVector(HalfX / 50.f, WallT / 100.f, WallH / 100.f));
        AddPrimitive(TEXT("WallSouth"), FVector(0.f, -HalfY, WallH * 0.5f), FVector(HalfX / 50.f, WallT / 100.f, WallH / 100.f));
        AddPrimitive(TEXT("WallEast"), FVector(HalfX, 0.f, WallH * 0.5f), FVector(WallT / 100.f, HalfY / 50.f, WallH / 100.f));
        AddPrimitive(TEXT("WallWest"), FVector(-HalfX, 0.f, WallH * 0.5f), FVector(WallT / 100.f, HalfY / 50.f, WallH / 100.f));
    }

    if (HasPrimitive(TEXT("posts")))
    {
        const FVector Corners[] = {
            FVector(HalfX, HalfY, WallH * 0.5f), FVector(-HalfX, HalfY, WallH * 0.5f),
            FVector(HalfX, -HalfY, WallH * 0.5f), FVector(-HalfX, -HalfY, WallH * 0.5f)
        };
        for (int32 i = 0; i < UE_ARRAY_COUNT(Corners); ++i)
        {
            AddPrimitive(*FString::Printf(TEXT("Post_%d"), i), Corners[i], FVector(0.14f, 0.14f, WallH / 100.f));
        }
    }

    if (HasPrimitive(TEXT("roof")))
    {
        const float RoofZ = WallH + 48.f;
        AddPrimitive(TEXT("RoofNorth"), FVector(0.f, HalfY * 0.48f, RoofZ), FVector(HalfX / 45.f, HalfY / 82.f, 0.12f), FRotator(18.f, 0.f, 0.f));
        AddPrimitive(TEXT("RoofSouth"), FVector(0.f, -HalfY * 0.48f, RoofZ), FVector(HalfX / 45.f, HalfY / 82.f, 0.12f), FRotator(-18.f, 0.f, 0.f));
    }

    if (HasPrimitive(TEXT("hearth"))) AddPrimitive(TEXT("Hearth"), FVector(0.f, 0.f, 18.f), FVector(0.5f, 0.5f, 0.18f));

    if (HasPrimitive(TEXT("doorway")))
    {
        AddPrimitive(TEXT("DoorPostL"), FVector(-55.f, -HalfY - 6.f, 105.f), FVector(0.12f, 0.12f, 1.05f));
        AddPrimitive(TEXT("DoorPostR"), FVector(55.f, -HalfY - 6.f, 105.f), FVector(0.12f, 0.12f, 1.05f));
        AddPrimitive(TEXT("DoorLintel"), FVector(0.f, -HalfY - 6.f, 205.f), FVector(0.68f, 0.12f, 0.12f));
    }

    if (HasPrimitive(TEXT("storage_bins")))
    {
        AddPrimitive(TEXT("StorageBinA"), FVector(-HalfX * 0.35f, 0.f, 55.f), FVector(0.6f, 0.6f, 0.55f));
        AddPrimitive(TEXT("StorageBinB"), FVector(HalfX * 0.35f, 0.f, 55.f), FVector(0.6f, 0.6f, 0.55f));
    }

    if (HasPrimitive(TEXT("drying_beams"))) AddPrimitive(TEXT("DryingBeam"), FVector(0.f, 0.f, 180.f), FVector(HalfX / 65.f, 0.1f, 0.1f));

    if (HasPrimitive(TEXT("fence")))
    {
        AddPrimitive(TEXT("FenceNorth"), FVector(0.f, HalfY, 60.f), FVector(HalfX / 50.f, 0.08f, 0.6f));
        AddPrimitive(TEXT("FenceSouth"), FVector(0.f, -HalfY, 60.f), FVector(HalfX / 50.f, 0.08f, 0.6f));
    }

    if (HasPrimitive(TEXT("platform"))) AddPrimitive(TEXT("Platform"), FVector(0.f, 0.f, 45.f), FVector(HalfX / 50.f, HalfY / 50.f, 0.18f));
    if (HasPrimitive(TEXT("pit"))) AddPrimitive(TEXT("PitMarker"), FVector(0.f, 0.f, -22.f), FVector(FMath::Min(HalfX, 120.f) / 50.f, FMath::Min(HalfY, 120.f) / 50.f, 0.12f));
    if (HasPrimitive(TEXT("channel"))) AddPrimitive(TEXT("Channel"), FVector(0.f, 0.f, 8.f), FVector(HalfX / 50.f, 0.3f, 0.08f));
    if (HasPrimitive(TEXT("work_surface"))) AddPrimitive(TEXT("WorkSurface"), FVector(0.f, 0.f, 80.f), FVector(1.1f, 0.55f, 0.12f));
}
