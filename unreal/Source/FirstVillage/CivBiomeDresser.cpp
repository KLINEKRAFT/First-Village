#include "CivBiomeDresser.h"
#include "CivRuntimeEnvironment.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
UMaterialInstanceDynamic* MakeTint(UObject* Outer, const FLinearColor& Color)
{
    UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (!Base) return nullptr;
    UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Outer);
    if (MID) MID->SetVectorParameterValue(TEXT("Color"), Color);
    return MID;
}

void SetTint(UHierarchicalInstancedStaticMeshComponent* Component, UObject* Outer, const FLinearColor& Color)
{
    if (Component)
    {
        if (UMaterialInstanceDynamic* MID = MakeTint(Outer, Color)) Component->SetMaterial(0, MID);
    }
}
}

ACivBiomeDresser::ACivBiomeDresser()
{
    PrimaryActorTick.bCanEverTick = false;

    TreeTrunks = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TreeTrunks"));
    RootComponent = TreeTrunks;
    TreeCrowns = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TreeCrowns"));
    TreeCrowns->SetupAttachment(RootComponent);
    Rocks = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Rocks"));
    Rocks->SetupAttachment(RootComponent);
    Shrubs = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Shrubs"));
    Shrubs->SetupAttachment(RootComponent);
    Reeds = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Reeds"));
    Reeds->SetupAttachment(RootComponent);
    Grass = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Grass"));
    Grass->SetupAttachment(RootComponent);

    for (UHierarchicalInstancedStaticMeshComponent* Component : { TreeTrunks.Get(), TreeCrowns.Get(), Rocks.Get(), Shrubs.Get(), Reeds.Get(), Grass.Get() })
    {
        if (Component) Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    UStaticMesh* Cone = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone"));
    UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    TreeTrunks->SetStaticMesh(Cylinder);
    TreeCrowns->SetStaticMesh(Cone);
    Rocks->SetStaticMesh(Sphere);
    Shrubs->SetStaticMesh(Sphere);
    Reeds->SetStaticMesh(Cylinder);
    Grass->SetStaticMesh(Cone);

    SetTint(TreeTrunks, this, FLinearColor(0.16f, 0.085f, 0.035f, 1.f));
    SetTint(TreeCrowns, this, FLinearColor(0.10f, 0.25f, 0.075f, 1.f));
    SetTint(Rocks, this, FLinearColor(0.28f, 0.28f, 0.24f, 1.f));
    SetTint(Shrubs, this, FLinearColor(0.16f, 0.31f, 0.09f, 1.f));
    SetTint(Reeds, this, FLinearColor(0.36f, 0.42f, 0.16f, 1.f));
    SetTint(Grass, this, FLinearColor(0.24f, 0.35f, 0.12f, 1.f));
}

void ACivBiomeDresser::BeginPlay()
{
    Super::BeginPlay();
    ScatterInstances();
}

void ACivBiomeDresser::ScatterInstances()
{
    UWorld* World = GetWorld();
    if (!World) return;
    ACivRuntimeEnvironment* Env = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(World, ACivRuntimeEnvironment::StaticClass()));
    if (!Env) return;

    TreeTrunks->ClearInstances(); TreeCrowns->ClearInstances(); Rocks->ClearInstances();
    Shrubs->ClearInstances(); Reeds->ClearInstances(); Grass->ClearInstances();

    FRandomStream Random(1337);
    const float RiverX = Env->GetRiverCenterX();

    for (int32 i = 0; i < TreeCount; ++i)
    {
        const float A = Random.FRandRange(0.f, 2.f * PI);
        const float R = FMath::Sqrt(Random.FRand()) * Radius;
        float X = FMath::Cos(A) * R;
        float Y = FMath::Sin(A) * R;

        if (FVector2D(X, Y).Size() < 1050.f) continue;
        if (FMath::Abs(X - RiverX) < 420.f) continue;

        if (i % 3 != 0)
        {
            X += X >= 0.f ? 600.f : -400.f;
        }

        const float Z = Env->GetTerrainZAtWorldXY(X, Y);
        const float Scale = Random.FRandRange(0.65f, 1.45f);
        TreeTrunks->AddInstance(FTransform(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f), FVector(X, Y, Z + 90.f * Scale), FVector(0.17f * Scale, 0.17f * Scale, 1.8f * Scale)));
        TreeCrowns->AddInstance(FTransform(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f), FVector(X, Y, Z + 255.f * Scale), FVector(1.05f * Scale, 1.05f * Scale, 1.65f * Scale)));
    }

    for (int32 i = 0; i < RockCount; ++i)
    {
        const float A = Random.FRandRange(0.f, 2.f * PI);
        const float R = FMath::Sqrt(Random.FRand()) * Radius;
        const float X = FMath::Cos(A) * R;
        const float Y = FMath::Sin(A) * R;
        if (FVector2D(X, Y).Size() < 700.f) continue;
        const float Z = Env->GetTerrainZAtWorldXY(X, Y);
        const float Scale = Random.FRandRange(0.25f, 0.95f);
        Rocks->AddInstance(FTransform(FRotator(Random.FRandRange(-12.f, 12.f), Random.FRandRange(0.f, 360.f), 0.f), FVector(X, Y, Z + 25.f * Scale), FVector(Scale, Scale * Random.FRandRange(0.7f, 1.25f), Scale * 0.62f)));
    }

    for (int32 i = 0; i < ShrubCount; ++i)
    {
        const float A = Random.FRandRange(0.f, 2.f * PI);
        const float R = FMath::Sqrt(Random.FRand()) * 4100.f;
        const float X = FMath::Cos(A) * R;
        const float Y = FMath::Sin(A) * R;
        if (FVector2D(X, Y).Size() < 650.f) continue;
        if (FMath::Abs(X - RiverX) < 260.f) continue;
        const float Z = Env->GetTerrainZAtWorldXY(X, Y);
        const float S = Random.FRandRange(0.18f, 0.48f);
        Shrubs->AddInstance(FTransform(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f), FVector(X, Y, Z + 20.f), FVector(S * 1.5f, S, S)));
    }

    for (int32 i = 0; i < ReedCount; ++i)
    {
        const float Y = Random.FRandRange(-4800.f, 4800.f);
        const float X = RiverX + Random.FRandRange(-330.f, 330.f) + FMath::Sin(Y * 0.0011f) * 150.f;
        const float Z = Env->GetTerrainZAtWorldXY(X, Y);
        const float H = Random.FRandRange(0.28f, 0.62f);
        Reeds->AddInstance(FTransform(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f), FVector(X, Y, Z + 28.f), FVector(0.035f, 0.035f, H)));
    }

    for (int32 i = 0; i < GrassCount; ++i)
    {
        const float A = Random.FRandRange(0.f, 2.f * PI);
        const float R = FMath::Sqrt(Random.FRand()) * 4200.f;
        const float X = FMath::Cos(A) * R;
        const float Y = FMath::Sin(A) * R;
        if (FMath::Abs(X - RiverX) < 220.f) continue;
        const float Z = Env->GetTerrainZAtWorldXY(X, Y);
        const float S = Random.FRandRange(0.06f, 0.16f);
        Grass->AddInstance(FTransform(FRotator(0.f, Random.FRandRange(0.f, 360.f), 0.f), FVector(X, Y, Z + 10.f), FVector(S, S, S * 2.2f)));
    }
}
