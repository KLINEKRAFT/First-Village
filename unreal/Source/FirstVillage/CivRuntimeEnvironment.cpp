#include "CivRuntimeEnvironment.h"
#include "ProceduralMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Materials/MaterialInterface.h"

ACivRuntimeEnvironment::ACivRuntimeEnvironment()
{
    PrimaryActorTick.bCanEverTick = false;

    Terrain = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain"));
    RootComponent = Terrain;
    Terrain->bUseAsyncCooking = false;
    Terrain->SetCollisionProfileName(TEXT("BlockAll"));

    Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
    Sun->SetupAttachment(RootComponent);
    Sun->SetRelativeRotation(FRotator(-48.f, -32.f, 0.f));
    Sun->SetIntensity(7.2f);
    Sun->SetUseTemperature(true);
    Sun->SetTemperature(5100.f);
    Sun->SetForwardShadingPriority(1);
    Sun->SetAtmosphereSunLight(true);

    SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
    SkyLight->SetupAttachment(RootComponent);
    SkyLight->SetIntensity(0.85f);
    SkyLight->SetRealTimeCaptureEnabled(true);

    SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
    SkyAtmosphere->SetupAttachment(RootComponent);

    Fog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("Fog"));
    Fog->SetupAttachment(RootComponent);
    Fog->FogDensity = 0.0045f;
    Fog->FogHeightFalloff = 0.18f;
}

void ACivRuntimeEnvironment::BeginPlay()
{
    Super::BeginPlay();
    GenerateValley();
}

void ACivRuntimeEnvironment::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    GenerateValley();
}

float ACivRuntimeEnvironment::GetWorldExtent() const
{
    return FMath::Max(1.f, GridResolution * CellSize * 0.5f);
}

float ACivRuntimeEnvironment::GetRiverCenterX() const
{
    return GetActorLocation().X - GetWorldExtent() * 0.38f;
}

float ACivRuntimeEnvironment::SampleHeight(float X, float Y) const
{
    const float Extent = GetWorldExtent();
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
    const float LocalRiverX = -GetWorldExtent() * 0.38f;

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
            const FVector Normal = FVector(-HX, -HY, Epsilon * 2.f).GetSafeNormal();
            Normals.Add(Normal);

            UV0.Add(FVector2D(static_cast<float>(X) / Resolution, static_cast<float>(Y) / Resolution));

            const float Height01 = FMath::Clamp((Z + 180.f) / (HeightScale + 300.f), 0.f, 1.f);
            const float River01 = 1.f - FMath::Clamp(FMath::Abs(PX - LocalRiverX) / 650.f, 0.f, 1.f);
            const float Slope = 1.f - FMath::Clamp(Normal.Z, 0.f, 1.f);
            const float CenterClearing = 1.f - FMath::Clamp(FVector2D(PX, PY).Size() / 1450.f, 0.f, 1.f);

            FLinearColor Ground = FLinearColor(0.19f, 0.27f, 0.12f, 1.f);
            Ground = FMath::Lerp(Ground, FLinearColor(0.31f, 0.28f, 0.16f, 1.f), CenterClearing * 0.55f);
            Ground = FMath::Lerp(Ground, FLinearColor(0.18f, 0.24f, 0.12f, 1.f), River01 * 0.75f);
            Ground = FMath::Lerp(Ground, FLinearColor(0.30f, 0.30f, 0.26f, 1.f), FMath::Clamp(Slope * 1.6f + Height01 * 0.35f, 0.f, 0.85f));
            Colors.Add(Ground);
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
    Terrain->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, Colors, Tangents, true, false);

    if (UMaterialInterface* VertexColorMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/EngineDebugMaterials/VertexColorMaterial.VertexColorMaterial")))
    {
        Terrain->SetMaterial(0, VertexColorMaterial);
    }
}
