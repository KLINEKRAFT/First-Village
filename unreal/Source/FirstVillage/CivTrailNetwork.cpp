#include "CivTrailNetwork.h"
#include "CivRuntimeEnvironment.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

ACivTrailNetwork::ACivTrailNetwork()
{
    PrimaryActorTick.bCanEverTick = false;
    Segments = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TrailSegments"));
    RootComponent = Segments;
    Segments->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
    {
        Segments->SetStaticMesh(Cube);
    }

    if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
    {
        if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, this))
        {
            MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.22f, 0.14f, 0.07f, 1.f));
            Segments->SetMaterial(0, MID);
        }
    }
}

void ACivTrailNetwork::AddTrail(const FVector& Start, const FVector& End, ACivRuntimeEnvironment* Environment, float Width)
{
    if (!Environment || !Segments) return;

    const FVector2D Delta2D(End.X - Start.X, End.Y - Start.Y);
    const float Distance = Delta2D.Size();
    if (Distance < 50.f) return;

    const int32 Count = FMath::Clamp(FMath::CeilToInt(Distance / 180.f), 2, 48);
    const float Step = Distance / Count;
    const float Yaw = FMath::RadiansToDegrees(FMath::Atan2(Delta2D.Y, Delta2D.X));

    for (int32 i = 0; i < Count; ++i)
    {
        const float Alpha = (i + 0.5f) / Count;
        FVector P = FMath::Lerp(Start, End, Alpha);
        P.Z = Environment->GetTerrainZAtWorldXY(P.X, P.Y) + 6.f;
        const FVector Scale(Step / 100.f * 0.58f, Width / 100.f, 0.025f);
        Segments->AddInstance(FTransform(FRotator(0.f, Yaw, 0.f), P, Scale));
    }
}
