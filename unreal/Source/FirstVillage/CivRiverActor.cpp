#include "CivRiverActor.h"
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
}

ACivRiverActor::ACivRiverActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Water = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Water"));
    RootComponent = Water;
    Water->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    BankStones = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("BankStones"));
    BankStones->SetupAttachment(RootComponent);
    BankStones->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"))) Water->SetStaticMesh(Cube);
    if (UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"))) BankStones->SetStaticMesh(Sphere);
}

void ACivRiverActor::BeginPlay()
{
    Super::BeginPlay();
    BuildRiver();
}

void ACivRiverActor::BuildRiver()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ACivRuntimeEnvironment* Env = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(World, ACivRuntimeEnvironment::StaticClass()));
    if (!Env) return;

    Water->ClearInstances();
    BankStones->ClearInstances();

    if (UMaterialInstanceDynamic* WaterMat = MakeTint(this, FLinearColor(0.06f, 0.19f, 0.24f, 1.f))) Water->SetMaterial(0, WaterMat);
    if (UMaterialInstanceDynamic* StoneMat = MakeTint(this, FLinearColor(0.25f, 0.25f, 0.21f, 1.f))) BankStones->SetMaterial(0, StoneMat);

    const float RiverX = Env->GetRiverCenterX();
    const float Extent = Env->GetWorldExtent();
    const int32 SegmentCount = 30;
    const float SegmentLength = (Extent * 2.f) / SegmentCount;

    for (int32 i = 0; i < SegmentCount; ++i)
    {
        const float Y = -Extent + SegmentLength * (i + 0.5f);
        const float Meander = FMath::Sin(Y * 0.0011f) * 150.f + FMath::Sin(Y * 0.00037f) * 90.f;
        const float X = RiverX + Meander;
        const float Z = Env->GetTerrainZAtWorldXY(X, Y) + 12.f;
        const float Width = 560.f + FMath::Sin(Y * 0.0017f) * 110.f;
        Water->AddInstance(FTransform(FRotator::ZeroRotator, FVector(X, Y, Z), FVector(Width / 100.f, SegmentLength / 100.f * 0.58f, 0.035f)));

        if (i % 2 == 0)
        {
            for (int32 Side : { -1, 1 })
            {
                const float SX = X + Side * (Width * 0.62f + 55.f);
                const float SZ = Env->GetTerrainZAtWorldXY(SX, Y) + 20.f;
                BankStones->AddInstance(FTransform(FRotator(0.f, i * 37.f, 0.f), FVector(SX, Y, SZ), FVector(0.36f, 0.28f, 0.18f)));
            }
        }
    }
}
