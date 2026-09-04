#include "CivBiomeDresser.h"
#include "CivRuntimeEnvironment.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ACivBiomeDresser::ACivBiomeDresser()
{
    PrimaryActorTick.bCanEverTick = false;

    TreeTrunks = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TreeTrunks"));
    RootComponent = TreeTrunks;
    TreeTrunks->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TreeCrowns = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("TreeCrowns"));
    TreeCrowns->SetupAttachment(RootComponent);
    TreeCrowns->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Rocks = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Rocks"));
    Rocks->SetupAttachment(RootComponent);
    Rocks->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"))) TreeTrunks->SetStaticMesh(Cylinder);
    if (UStaticMesh* Cone = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone"))) TreeCrowns->SetStaticMesh(Cone);
    if (UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"))) Rocks->SetStaticMesh(Sphere);
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

    FRandomStream Random(1337);
    for (int32 i = 0; i < TreeCount; ++i)
    {
        const float A = Random.FRandRange(0.f, 2.f * PI);
        const float R = FMath::Sqrt(Random.FRand()) * Radius;
        const float X = FMath::Cos(A) * R;
        const float Y = FMath::Sin(A) * R;
        if (FVector2D(X, Y).Size() < 850.f) continue;
        const float Z = Env->GetTerrainZAtWorldXY(X, Y);
        const float Scale = Random.FRandRange(0.65f, 1.35f);
        TreeTrunks->AddInstance(FTransform(FRotator::ZeroRotator, FVector(X, Y, Z + 90.f * Scale), FVector(0.18f * Scale, 0.18f * Scale, 1.8f * Scale)));
        TreeCrowns->AddInstance(FTransform(FRotator::ZeroRotator, FVector(X, Y, Z + 255.f * Scale), FVector(1.1f * Scale, 1.1f * Scale, 1.7f * Scale)));
    }

    for (int32 i = 0; i < RockCount; ++i)
    {
        const float A = Random.FRandRange(0.f, 2.f * PI);
        const float R = FMath::Sqrt(Random.FRand()) * Radius;
        const float X = FMath::Cos(A) * R;
        const float Y = FMath::Sin(A) * R;
        const float Z = Env->GetTerrainZAtWorldXY(X, Y);
        const float Scale = Random.FRandRange(0.25f, 0.9f);
        Rocks->AddInstance(FTransform(FRotator(Random.FRandRange(-12.f, 12.f), Random.FRandRange(0.f, 360.f), 0.f), FVector(X, Y, Z + 25.f * Scale), FVector(Scale, Scale * Random.FRandRange(0.7f, 1.2f), Scale * 0.65f)));
    }
}
