#include "CivProceduralBuilding.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ACivProceduralBuilding::ACivProceduralBuilding()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
}

void ACivProceduralBuilding::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    RebuildFromBlueprint();
}

void ACivProceduralBuilding::ClearGeneratedParts()
{
    for (UStaticMeshComponent* Part : GeneratedParts)
    {
        if (Part)
        {
            Part->DestroyComponent();
        }
    }
    GeneratedParts.Reset();
}

UStaticMeshComponent* ACivProceduralBuilding::AddPrimitive(const FName& Name, const FVector& RelativeLocation, const FVector& Scale, const FRotator& Rotation)
{
    UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(this, Name);
    Part->SetupAttachment(Root);
    Part->RegisterComponent();
    Part->SetRelativeLocation(RelativeLocation);
    Part->SetRelativeRotation(Rotation);
    Part->SetRelativeScale3D(Scale);
    Part->SetCollisionProfileName(TEXT("BlockAll"));

    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube)
    {
        Part->SetStaticMesh(Cube);
    }

    GeneratedParts.Add(Part);
    return Part;
}

void ACivProceduralBuilding::RebuildFromBlueprint()
{
    ClearGeneratedParts();

    const float HalfX = FMath::Max(100.f, Blueprint.Footprint.X * 0.5f);
    const float HalfY = FMath::Max(100.f, Blueprint.Footprint.Y * 0.5f);
    const float WallT = 18.f;
    const float WallH = FMath::Clamp(Blueprint.WallHeight, 150.f, 500.f);

    AddPrimitive(TEXT("WallNorth"), FVector(0.f, HalfY, WallH * 0.5f), FVector(HalfX / 50.f, WallT / 100.f, WallH / 100.f));
    AddPrimitive(TEXT("WallSouth"), FVector(0.f, -HalfY, WallH * 0.5f), FVector(HalfX / 50.f, WallT / 100.f, WallH / 100.f));
    AddPrimitive(TEXT("WallEast"), FVector(HalfX, 0.f, WallH * 0.5f), FVector(WallT / 100.f, HalfY / 50.f, WallH / 100.f));
    AddPrimitive(TEXT("WallWest"), FVector(-HalfX, 0.f, WallH * 0.5f), FVector(WallT / 100.f, HalfY / 50.f, WallH / 100.f));

    const int32 Posts = FMath::Clamp(Blueprint.PostCount, 0, 24);
    for (int32 i = 0; i < Posts; ++i)
    {
        const float Alpha = Posts > 1 ? static_cast<float>(i) / static_cast<float>(Posts - 1) : 0.f;
        const float X = FMath::Lerp(-HalfX, HalfX, Alpha);
        AddPrimitive(*FString::Printf(TEXT("Post_%d"), i), FVector(X, -HalfY, WallH * 0.5f), FVector(0.12f, 0.12f, WallH / 100.f));
    }

    if (Blueprint.bHasRoof)
    {
        AddPrimitive(TEXT("Roof"), FVector(0.f, 0.f, WallH + 35.f), FVector(HalfX / 45.f, HalfY / 45.f, 0.12f), FRotator(0.f, 0.f, 0.f));
    }

    if (Blueprint.bHasHearth)
    {
        AddPrimitive(TEXT("Hearth"), FVector(0.f, 0.f, 20.f), FVector(0.45f, 0.45f, 0.18f));
    }
}
