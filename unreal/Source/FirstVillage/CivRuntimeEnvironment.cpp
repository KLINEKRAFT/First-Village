#include "CivRuntimeEnvironment.h"
#include "ProceduralMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"

ACivRuntimeEnvironment::ACivRuntimeEnvironment()
{
    PrimaryActorTick.bCanEverTick = false;

    Terrain = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain"));
    RootComponent = Terrain;
    Terrain->bUseAsyncCooking = true;
    Terrain->SetCollisionProfileName(TEXT("BlockAll"));

    Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
    Sun->SetupAttachment(RootComponent);
    Sun->SetRelativeRotation(FRotator(-48.f, -32.f, 0.f));
    Sun->Intensity = 8.5f;
    Sun->bUseTemperature = true;
    Sun->Temperature = 5400.f;

    SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
    SkyLight->SetupAttachment(RootComponent);
    SkyLight->Intensity = 1.0f;
    SkyLight->bRealTimeCapture = true;

    Fog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("Fog"));
    Fog->SetupAttachment(RootComponent);
    Fog->FogDensity = 0.008f;
    Fog->FogHeightFalloff = 0.2f;
}

void ACivRuntimeEnvironment::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    GenerateValley();
}

float ACivRuntimeEnvironment::SampleHeight(float X, float Y) const
{
    const float Extent = FMath::Max(1.f, GridResolution * CellSize * 0.5f);
    const float NX = X / Extent;
    const float NY = Y / Extent;
    const float Radius = FMath::Sqrt(NX * NX + NY * NY);

    const float Basin = FMath::Pow(FMath::Clamp(Radius, 0.f, 1.4f), 1.8f) * HeightScale;
    const float RidgeA = FMath::Sin(X * 0.00075f) * FMath::Cos(Y * 0.00055f) * HeightScale * 0.18f;
    const float RidgeB = FMath::Sin((X + Y) * 0.00038f) * HeightScale * 0.12f;

    const float RiverCenterX = -Extent * 0.38f;
    const float RiverDistance = FMath::Abs(X - RiverCenterX);
    const float RiverCut = FMath::Exp(-FMath::Square(RiverDistance / 520.f)) * HeightScale * 0.22f;

    return Basin + RidgeA + RidgeB - RiverCut - 120.f;
}

float ACivRuntimeEnvironment::GetTerrainZAtWorldXY(float WorldX, float WorldY) const
{
    const FVector Origin = GetActorLocation();
    return Origin.Z + SampleHeight(WorldX - Origin.X, WorldY - Origin.Y);
}

void ACivRuntimeEnvironment::GenerateValley()
{
    if (!Terrain) return;

    const int32 Resolution = FMath::Clamp(GridResolution, 8, 160);
    const float Step = FMath::Max(CellSize, 50.f);
    const float Half = Resolution * Step * 0.5f;

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FLinearColor> Colors;
    TArray<FProcMeshTangent> Tangents;

    const int32 VertexCount = (Resolution + 1) * (Resolution + 1);
    Vertices.Reserve(VertexCount);
    Normals.Reserve(VertexCount);
    UV0.Reserve(VertexCount);
    Colors.Reserve(VertexCount);
    Tangents.Reserve(VertexCount);

    for (int32 Y = 0; Y <= Resolution; ++Y)
    {
        for (int32 X = 0; X <= Resolution; ++X)
        {
            const float PX = -Half + X * Step;
            const float PY = -Half + Y * Step;
            const float Z = SampleHeight(PX, PY);
            Vertices.Add(FVector(PX, PY, Z));

            const float Epsilon = Step * 0.5f;
            const float HX = SampleHeight(PX + Epsilon, PY) - SampleHeight(PX - Epsilon, PY);
            const float HY = SampleHeight(PX, PY + Epsilon) - SampleHeight(PX, PY - Epsilon);
            Normals.Add(FVector(-HX, -HY, Epsilon * 2.f).GetSafeNormal());

            UV0.Add(FVector2D(static_cast<float>(X) / Resolution, static_cast<float>(Y) / Resolution));

            const float Height01 = FMath::Clamp((Z + 200.f) / (HeightScale + 350.f), 0.f, 1.f);
            Colors.Add(FLinearColor(0.18f + Height01 * 0.12f, 0.28f + Height01 * 0.12f, 0.12f + Height01 * 0.06f, 1.f));
            Tangents.Add(FProcMeshTangent(1.f, 0.f, 0.f));
        }
    }

    for (int32 Y = 0; Y < Resolution; ++Y)
    {
        for (int32 X = 0; X < Resolution; ++X)
        {
            const int32 A = Y * (Resolution + 1) + X;
            const int32 B = A + 1;
            const int32 C = A + Resolution + 1;
            const int32 D = C + 1;

            Triangles.Add(A); Triangles.Add(C); Triangles.Add(B);
            Triangles.Add(B); Triangles.Add(C); Triangles.Add(D);
        }
    }

    Terrain->ClearAllMeshSections();
    Terrain->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, Colors, Tangents, true);
}
